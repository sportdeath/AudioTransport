#include <iostream>
#include <complex>

#include <AudioUtilities/CoordinateTransforms.hpp>
#include <AudioUtilities/Plotting.hpp>

#include "Vocoder/PhaseVocoder.hpp"
#include "Vocoder/BidirectionalTransportVocoder.hpp"
#include "Vocoder/Transport.hpp"

BidirectionalTransportVocoder::BidirectionalTransportVocoder(
   unsigned int hopSize_, 
   unsigned int windowSize_,
   unsigned int sampleRate_
   ) : 
  PhaseVocoder(hopSize_, windowSize_, 1, sampleRate_),
  hopSize(hopSize_),
  windowSize(windowSize_),
  sampleRate(sampleRate_)
{
  amplitudes0 = new double[transformSize];
  amplitudes1 = new double[transformSize];
  phases0 = new double[transformSize];
  phases1 = new double[transformSize];

  accumulatedPhases0 = new double[transformSize];
  accumulatedPhases1 = new double[transformSize];
  for (std::size_t i = 0; i < transformSize; i++) {
    accumulatedPhases0[i] = 0;
    accumulatedPhases1[i] = 0;
  }

  masses0 = new SpectralMass[transformSize];
  masses1 = new SpectralMass[transformSize];
  massesOut = new SpectralMass[transformSize];

  // This is maximum possible number of assignments
  // in the transform
  int maxTransportSize = 2*transformSize - 1;

  massesAmpData = new double[maxTransportSize];
  massesPhaseData = new double[maxTransportSize];

  assignmentIndices0 = new std::size_t[maxTransportSize];
  assignmentIndices1 = new std::size_t[maxTransportSize];
  assignmentMasses = new double[maxTransportSize];
}

BidirectionalTransportVocoder::~BidirectionalTransportVocoder() {
  delete [] amplitudes0;
  delete [] amplitudes1;
  delete [] phases0;
  delete [] phases1;
  delete [] accumulatedPhases0;
  delete [] accumulatedPhases1;
  delete [] masses0;
  delete [] masses1;
  delete [] massesOut;
  delete [] massesAmpData;
  delete [] massesPhaseData;
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

  // Accumulate phases
  accumulatePhases(
      phases0,
      transforms0,
      accumulatedPhases0
      );
  accumulatePhases(
      phases1,
      transforms1,
      accumulatedPhases1
      );

  // Segment masses
  std::size_t numMasses0 = segementIntoMasses(
      transforms0,
      amplitudes0,
      accumulatedPhases0,
      masses0);
  std::size_t numMasses1 = segementIntoMasses(
      transforms1,
      amplitudes1,
      accumulatedPhases1,
      masses1);

  //if (interpolationFactor > 0.5) {
    //plotSegmentation(transforms0, masses0, numMasses0);
  //}

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

  massesOut[0].centerOfMassAmp = massesAmpData;
  massesOut[0].centerOfMassPhase = massesPhaseData;

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

  //if (interpolationFactor > 0.5) {
    //plotSegmentation(transforms0, masses0, numMasses0);
    //plotSegmentation(transforms1, masses1, numMasses1);

    //std::vector<double> amps(transformSize);
    //for (int i = 0; i < transformSize; i++) {
      //amps[i] = 0.001 * std::abs(transformOut[i]);
    //}

    //std::vector<double> phases(transformSize);
    //for (int i = 0; i < transformSize; i++) {
      //phases[i] = std::arg(transformOut[i]);
    //}

    //std::vector<std::vector<double> > vectors = {
      //phases,
      //amps
    //};

    //Plotting::plotVectors(vectors);
  //}
}

void BidirectionalTransportVocoder::accumulatePhases(
    const double * phases,
    std::complex<double> ** transforms,
    double * accumulatedPhases
    ) {

  for (std::size_t i = 0; i < transformSize; i++) {
    // Accumulate the phase
    accumulatedPhases[i] +=
      hopSize/double(sampleRate) * 
      (getFrequencyAdjustment(transforms, i) + getBinFrequency(i));
    
    // Push it to be the target phase
    double difference = 
      phases[i] - std::fmod(accumulatedPhases[i] + M_PI,2*M_PI) + M_PI;
    // Now between - pi and pi
    difference = std::fmod(difference + M_PI,2*M_PI) - M_PI;
    accumulatedPhases[i] += difference;
  }
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
      transform[outputIndex] += 
        std::polar(
            (mass.centerOfMassAmp + offset)[0],
            (mass.centerOfMassPhase + offset)[0]);

    }
  }

}

std::size_t BidirectionalTransportVocoder::segementIntoMasses(
    std::complex<double> ** transforms,
    double * amplitudes,
    double * accumulatedPhases,
    SpectralMass * masses
    ) {

  std::size_t numMasses = 1;

  masses[0].centerOfMass = 0;
  masses[0].centerOfMassAmp = amplitudes;
  masses[0].centerOfMassPhase = accumulatedPhases;
  masses[0].leftLength = 0;

  // Compute centers of masses
  double previousAdjustment = getFrequencyAdjustment(transforms, 0);
  for (std::size_t i = 1; i < transformSize; i++) {
    double adjustment = getFrequencyAdjustment(transforms, i);
    
    if (previousAdjustment >= 0 and adjustment <= 0) {
      // Center it to the closer one
      masses[numMasses - 1].centerOfMass = i - (previousAdjustment < -adjustment);
      // Put a pointer to the array element
      masses[numMasses - 1].centerOfMassAmp
        = amplitudes + masses[numMasses - 1].centerOfMass;
      masses[numMasses - 1].centerOfMassPhase
        = accumulatedPhases + masses[numMasses - 1].centerOfMass;
      // The left length is the
      // center of mass minus the start index
      masses[numMasses - 1].leftLength = 
        masses[numMasses - 1].centerOfMass - masses[numMasses - 1].leftLength;
      // Add it to the masses
    } else if (previousAdjustment < 0 and adjustment > 0) {
      // goes up until previous adjustment
      masses[numMasses - 1].rightLength = 
        i - 1 - masses[numMasses - 1].centerOfMass;

      // Make a new mass
      // Set the temporary left length
      // to equal the start index
      masses[numMasses].leftLength = i;
      numMasses++;
    }
    previousAdjustment = adjustment;
  }

  masses[numMasses-1].rightLength = 
    transformSize - 1 - masses[numMasses-1].centerOfMass;

  // Compute masses
  for(std::size_t massIndex = 0; massIndex < numMasses; massIndex++) {
    masses[massIndex].mass = 0;
    for(std::size_t i = 
        masses[massIndex].centerOfMass - masses[massIndex].leftLength;
        i <= masses[massIndex].centerOfMass + masses[massIndex].rightLength; 
        i++) {
      masses[massIndex].mass += std::abs(transforms[0][i]);
    }
  }

  return numMasses;
}

double BidirectionalTransportVocoder::getBinFrequency(
    std::size_t index
    ) {
  return 2. * M_PI * double(index) * sampleRate/double(windowSize);
}

double BidirectionalTransportVocoder::getFrequencyAdjustment(
    std::complex<double> ** transforms,
    std::size_t i
    ) {
  // SHOULD THIS
  // BE A PLUS OR A MINUS
  // PAPER SAYS PLUS
  //
  // Also for accumulation we need hopsize!!
  return - std::imag(
        (transforms[1][i] * std::conj(transforms[0][i])) / 
        (std::real(transforms[0][i]) * std::real(transforms[0][i]) +
         std::imag(transforms[0][i]) * std::imag(transforms[0][i])));
}

void BidirectionalTransportVocoder::plotSegmentation(
    std::complex<double> ** transforms,
    SpectralMass * masses,
    std::size_t numMasses
    ) {

  double AMPLIFY = 0.001;
  
  std::vector<double> reassigned(transformSize);
  for (int i = 0; i < transformSize; i++) {
    reassigned[i] = getFrequencyAdjustment(transforms, i) + getBinFrequency(i);
  }

  std::vector<double> frequency(transformSize);
  for (int i = 0; i < transformSize; i++) {
    frequency[i] = getBinFrequency(i);
  }

  std::vector<double> amps(transformSize);
  for (int i = 0; i < transformSize; i++) {
    amps[i] = AMPLIFY*std::abs(transforms[0][i]);
  }

  std::vector<double> phases(transformSize);
  for (int i = 0; i < transformSize; i++) {
    phases[i] = std::arg(transforms[0][i]);
  }

  std::vector<std::vector<double> > plotVectors = {
    //reassigned, 
    //frequency,
    phases,
    amps
  };

  std::vector<std::pair<double, double> > centers(numMasses);
  for (int i = 0; i < numMasses; i++) {
    std::size_t position = masses[i].centerOfMass;
    centers[i] = std::make_pair(position, AMPLIFY * std::abs(transforms[0][position]));
  }

  std::vector<std::pair<double, double> > lefts(numMasses);
  for (int i = 0; i < numMasses; i++) {
    std::size_t position = masses[i].centerOfMass - masses[i].leftLength;
    lefts[i] = std::make_pair(position, AMPLIFY * std::abs(transforms[0][position]));
  }

  std::vector<std::pair<double, double> > rights(numMasses);
  for (int i = 0; i < numMasses; i++) {
    std::size_t position = masses[i].centerOfMass + masses[i].rightLength;
    rights[i] = std::make_pair(position, AMPLIFY * std::abs(transforms[0][position]));
  }

  std::vector<std::vector<std::pair<double, double> > > plotMarkers = {
    centers,
    //lefts,
    //rights
  };

  std::cout << "Plotting!" << std::endl;
  Plotting::plotLinesAndMarkers(plotVectors, plotMarkers);
}
