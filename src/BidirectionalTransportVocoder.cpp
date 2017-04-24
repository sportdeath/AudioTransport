#include <iostream>
#include <complex>

#include <AudioUtilities/CoordinateTransforms.hpp>
#include <AudioUtilities/Plotting.hpp>

#include "Vocoder/PhaseVocoder.hpp"
#include "Vocoder/BidirectionalTransportVocoder.hpp"
#include "Vocoder/Transport.hpp"

BidirectionalTransportVocoder::BidirectionalTransportVocoder(
   unsigned int hopSize, 
   unsigned int windowSize,
   unsigned int sampleRate_
   ) : 
  PhaseVocoder(hopSize, windowSize, 2, sampleRate_),
  sampleRate(sampleRate_)
{
  amplitudes0 = new double[transformSize];
  amplitudes1 = new double[transformSize];
  phases0 = new double[transformSize];
  phases1 = new double[transformSize];

  masses0 = new SpectralMass[transformSize];
  masses1 = new SpectralMass[transformSize];
  massesOut = new SpectralMass[transformSize];

  // This is maximum possible number of assignments
  // in the transform
  int maxTransportSize = 2*transformSize - 1;

  massesData = new std::complex<double>[maxTransportSize];

  assignmentIndices0 = new std::size_t[maxTransportSize];
  assignmentIndices1 = new std::size_t[maxTransportSize];
  assignmentMasses = new double[maxTransportSize];
}

BidirectionalTransportVocoder::~BidirectionalTransportVocoder() {
  delete [] amplitudes0;
  delete [] amplitudes1;
  delete [] phases0;
  delete [] phases1;
  delete [] masses0;
  delete [] masses1;
  delete [] massesOut;
  delete [] assignmentIndices0;
  delete [] assignmentIndices1;
  delete [] assignmentMasses;
}

void BidirectionalTransportVocoder::processFrameTransform(
    std::complex<double> ** transforms0,
    std::complex<double> ** transforms1,
    double interpolationFactor, 
    std::complex<double> * transformOut
    ) {

  if (interpolationFactor > 0) {
    std::vector<double> reassigned(transformSize);
    for (int i = 0; i < transformSize; i++) {
      reassigned[i] = getReassignedFrequency(transforms0, i);
    }

    std::vector<double> frequency(transformSize);
    for (int i = 0; i < transformSize; i++) {
      frequency[i] = getBinFrequency(i);
    }

    std::vector<double> amps(transformSize);
    for (int i = 0; i < transformSize; i++) {
      amps[i] = 100*std::abs(transforms0[0][i]);
    }

    std::vector<std::vector<double> > plots = {
      reassigned, 
      frequency,
      amps
    };

    std::cout << "Plotting!" << std::endl;
    Plotting::plotVectors(plots);
  }

  // Convert to polar
  CoordinateTransforms::cartesianToPolar(
      transforms0[0],
      amplitudes0,
      phases0,
      transformSize
      );
  CoordinateTransforms::cartesianToPolar(
      transforms1[0],
      amplitudes1,
      phases1,
      transformSize
      );

  // Segment masses
  std::size_t numMasses0 = segementIntoMasses(
      transforms0,
      masses0);
  std::size_t numMasses1 = segementIntoMasses(
      transforms1,
      masses1);

  // Normalize masses
  double volume0 = 
    Transport::normalize<SpectralMass>(
      masses0, 
      transformSize,
      masses0
      );
  double volume1 = 
    Transport::normalize<SpectralMass>(
      masses1, 
      transformSize,
      masses1
      );

  // Compute transformation matrix
  std::size_t numAssignments = 
    Transport::massAssignment<SpectralMass>(
      masses0,
      masses1,
      numMasses0,
      numMasses1,
      assignmentIndices0,
      assignmentIndices1,
      assignmentMasses
      );

  massesOut[0].centerOfMassPointer = massesData;

  // Move mass along transformation matrix
  // According to interpolation factor
  Transport::transport<SpectralMass>(
      masses0,
      masses1,
      assignmentIndices0,
      assignmentIndices1,
      assignmentMasses,
      numAssignments,
      interpolationFactor,
      massesOut,
      transformSize
      );

  // Unsegment the masses
  unsegment(
      massesOut,
      numAssignments,
      transformOut
      );
}

void BidirectionalTransportVocoder::unsegment(
    const SpectralMass * masses,
    std::size_t numMasses,
    std::complex<double> * transform
    ) {
  // Clear the output
  for (std::size_t i = 0; i < transformSize; i++) {
    transform[i] = 0;
  }

  // for each assignment
  SpectralMass mass;
  for (std::size_t massIndex = 0; massIndex < numMasses; massIndex++) {
    mass = masses[massIndex];
    for (long offset = -long(mass.leftLength); offset <= long(mass.rightLength); offset++) {
      std::size_t outputIndex = mass.centerOfMass + offset;

      // add the data
      transform[outputIndex] += (mass.centerOfMassPointer + offset)[0];
    }
  }

}

std::size_t BidirectionalTransportVocoder::segementIntoMasses(
    std::complex<double> ** transforms,
    SpectralMass * masses
    ) {

  int polarState = 0;
  int pastState = 0;
  std::size_t zeroStart = 0;
  std::size_t polarStart = 0;

  int numMasses = 0;

  for (std::size_t i = 0; i < transformSize; i++) {
    double groupDelayDerivative = 
      getGroupDelayDerivative(transforms, i);

    // 1 > Threshold
    // 0  else
    // -1 < -Threshold
    int currentState = 
      (groupDelayDerivative > GROUP_DELAY_D_THRESHOLD) - 
      (groupDelayDerivative < -GROUP_DELAY_D_THRESHOLD);

    if (currentState != pastState) {
      if (currentState == 0) {
        zeroStart = i;
      } else if (currentState != polarState) {
        if (pastState == 0) {
          // Make a mass between
          // polarStart and zeroStart
          masses[numMasses] = getMass(transforms[0], polarStart, zeroStart);
          if (polarStart != zeroStart) numMasses += 1;

          // Make a mass between
          // zeroStart and i
          masses[numMasses] = getMass(transforms[0], zeroStart, i);
          if (zeroStart != i) numMasses += 1;
        } else {
          // Make a mass between
          // polarStart and i
          masses[numMasses] = getMass(transforms[0], polarStart, i);
          if (polarStart != i) numMasses += 1;
        }
        polarState = currentState;
        polarStart = i;
      }
    }

    pastState = currentState;
  }

  // Make a final mass
  // between polarStart and transformSize
  masses[numMasses] = getMass(transforms[0], polarStart, transformSize);
  if (polarStart != transformSize) numMasses += 1;

  return numMasses;
}

// Makes a mass [startIndex, endIndex)
SpectralMass BidirectionalTransportVocoder::getMass(
    std::complex<double> * transform,
    std::size_t startIndex,
    std::size_t endIndex
    ) {
  SpectralMass mass;

  mass.mass = 0;

  double centerOfMass = 0;
  for (std::size_t i = startIndex; i < endIndex; i++) {
    double magnitude = std::abs(transform[i]);
    mass.mass += magnitude;
    centerOfMass += magnitude * i;
  }
  mass.centerOfMass = std::round(centerOfMass/mass.mass);
  mass.leftLength = mass.centerOfMass - startIndex;
  mass.rightLength = endIndex - mass.centerOfMass - 1;
  mass.centerOfMassPointer = transform + mass.centerOfMass;

  return mass;
}

double BidirectionalTransportVocoder::getGroupDelayDerivative(
    std::complex<double> ** transforms,
    std::size_t i) {
  return (transforms[2][i]/transforms[0][i] -
    std::pow(transforms[1][i]/transforms[0][i], 2)).imag();
}

double BidirectionalTransportVocoder::getBinFrequency(
    std::size_t index
    ) {
  return 2 * M_PI * long(index) * sampleRate/(2. * (transformSize - 1.));
}

double BidirectionalTransportVocoder::getReassignedFrequency(
    std::complex<double> ** transforms,
    std::size_t i
    ) {
  return 
    getBinFrequency(i) - std::imag(
        (transforms[1][i] * std::conj(transforms[0][i])) / 
        (std::real(transforms[0][i]) * std::real(transforms[0][i]) +
         std::imag(transforms[0][i]) * std::imag(transforms[0][i])));
}
