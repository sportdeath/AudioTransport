#ifndef TRANSPORT_CPP
#define TRANSPORT_CPP

#include <cstddef>
#include <cmath>
#include <complex>

#include <iostream>

#include "Vocoder/Transport.hpp"
#include "Vocoder/SpectralMass.hpp"

template <> inline
double Transport::getMass<double>(
    const double mass
    ) {
  return mass;
}

template <> inline
double Transport::getMass<SpectralMass>(const SpectralMass mass) {
  return mass.mass;
}

template <> inline
void Transport::setMass<double>(
    double * masses, 
    std::size_t index, 
    double mass) {
  masses[index] = mass;
}

template <> inline
void Transport::setMass<SpectralMass>(
    SpectralMass * masses, 
    std::size_t index, 
    double mass) {
  masses[index].mass = mass;
}

template <class MassContainer>
double Transport::normalize(
    const MassContainer * massesIn, 
    std::size_t numMasses,
    MassContainer * massesOut) {
  // Compute total volume
  double volume = 0;
  for (std::size_t index = 0; index < numMasses; index++) {
    volume += getMass<MassContainer>(massesIn[index]);
  }
  
  if (volume == 0) {
    return 1;
  }

  // Divide by total volume
  for (std::size_t index = 0; index < numMasses; index++) {
    double newMass = getMass<MassContainer>(massesIn[index])/volume;
    setMass<MassContainer>(massesOut, index, newMass);
  }

  return volume;
}

template <class MassContainer>
void Transport::unnormalize(
    MassContainer * masses, 
    std::size_t numMasses,
    double volume0, 
    double volume1, 
    double interpolationFactor) {

  // interpolate to new volume
  double newVolume = volume0 * (1 - interpolationFactor) + volume1 * interpolationFactor;

  for (std::size_t index = 0; index < numMasses; index++) {
    double newMass = getMass<MassContainer>(masses[index]) * newVolume;
    setMass(masses, index, newMass);
  }
}

template <class MassContainer>
bool Transport::updateMass(
    const MassContainer * masses,
    const std::size_t numMasses,
    std::size_t & index, 
    double & mass
    ) {
  while (mass <= 0) {
    index++;
    if (index < numMasses) {
      mass = getMass<MassContainer>(masses[index]);
    } else {
      return false;
    }
  } 
  return true;
}
template <class MassContainer>
std::size_t Transport::massAssignment(
    const MassContainer * masses0, 
    const MassContainer * masses1,
    const std::size_t numMasses0,
    const std::size_t numMasses1,
    std::size_t * assignmentIndices0,
    std::size_t * assignmentIndices1,
    double * assignmentMasses
    ) {
  std::size_t numAssignments = 0;

  std::size_t index0 = 0;
  std::size_t index1 = 0;
  double mass0 = getMass<MassContainer>(masses0[index0]);
  double mass1 = getMass<MassContainer>(masses1[index1]);

  while (true) {
    // save the indexes
    assignmentIndices0[numAssignments] = index0;
    assignmentIndices1[numAssignments] = index1;

    // If mass 0 is small we use it up completely
    // otherwise we use mass 1 up completely
    if (mass0 < mass1) {
      assignmentMasses[numAssignments] = mass0;
    } else {
      assignmentMasses[numAssignments] = mass1;
    }

    // Subtract from mass
    // Guaranteed to be positive or zero
    mass0 -= assignmentMasses[numAssignments];
    mass1 -= assignmentMasses[numAssignments];

    // Update the number of assignments
    numAssignments++;

    // If either mass is now zero
    // update the index and the mass
    // If the index reaches the end, we break
    if (not updateMass<MassContainer>(masses0, numMasses0, index0, mass0)) break;
    if (not updateMass<MassContainer>(masses1, numMasses1, index1, mass1)) break;
  }

  return numAssignments;
}

//template <> inline
//void Transport::transport<double>(
    //const double * masses0,
    //const double * masses1,
    //const std::size_t * assignmentIndices0,
    //const std::size_t * assignmentIndices1,
    //const double * assignmentMasses,
    //const std::size_t numAssignments,
    //const double interpolationFactor,
    //double * massesOut,
    //const std::size_t numMasses
    //) {
  //// Clear the current output
  //for (std::size_t i = 0; i < numMasses; i++) {
    //massesOut[i] = 0;
  //}

  //for (std::size_t assignmentIndex = 0; assignmentIndex < numAssignments; assignmentIndex++) {
    //std::size_t assignment0 = assignmentIndices0[assignmentIndex];
    //std::size_t assignment1 = assignmentIndices1[assignmentIndex];

    //double assignment = assignment0 * (1 - interpolationFactor) + assignment1 * interpolationFactor;

    //int leftAssignment = std::floor(assignment);
    //int rightAssignment = std::floor(assignment+1);

    //double leftContribution = rightAssignment - assignment;
    //double rightContribution = assignment - leftAssignment;

    //massesOut[leftAssignment] += leftContribution * assignmentMasses[assignmentIndex];
    //massesOut[rightAssignment] += rightContribution * assignmentMasses[assignmentIndex];
  //}
//}

template <> inline
void Transport::transport<SpectralMass>(
    // The masses
    const SpectralMass * masses0,
    const SpectralMass * masses1,
    // The mass assignment
    const std::size_t * assignmentIndices0,
    const std::size_t * assignmentIndices1,
    const double * assignmentMasses,
    const std::size_t numAssignments,
    // The interpolation
    const double interpolationFactor,
    // The output
    std::complex<double> * transformOut,
    const std::size_t transformSize
    ) {

  for (int i = 0; i < transformSize; i++) {
    transformOut[i] = 0;
  }

  for (std::size_t assignmentIndex = 0; assignmentIndex < numAssignments; assignmentIndex++) {
    std::size_t massIndex0 = assignmentIndices0[assignmentIndex];
    std::size_t massIndex1 = assignmentIndices1[assignmentIndex];

    SpectralMass mass0 = masses0[massIndex0];
    SpectralMass mass1 = masses1[massIndex1];

    std::size_t centerOfMass = 
      std::round(mass0.centerOfMass * (1 - interpolationFactor) + mass1.centerOfMass * (interpolationFactor));

    // scale by assignment masses
    double scale0 = assignmentMasses[assignmentIndex]/mass0.mass;
    double scale1 = assignmentMasses[assignmentIndex]/mass1.mass;

    // compute new data
    std::size_t leftLength = std::min(mass0.leftLength, mass1.leftLength);
    std::size_t rightLength = std::min(mass0.rightLength, mass1.rightLength);

    std::size_t totalLength = leftLength + rightLength + 1;

    // Fill in new mass data

    double desiredPeakPhase = 
      mass0.centerOfMassPhase[0] * (1 - interpolationFactor) +
      mass1.centerOfMassPhase[0] * interpolationFactor;

    double phase0Adjustment = desiredPeakPhase - mass0.centerOfMassPhase[0];
    double phase1Adjustment = desiredPeakPhase - mass1.centerOfMassPhase[0];

    for (std::size_t i = 0; i < totalLength; i++) {
      long offset = i - leftLength;

      // Scale the amplitude
      double amp0 = scale0 * (mass0.centerOfMassAmp + offset)[0];
      double amp1 = scale1 * (mass1.centerOfMassAmp + offset)[0];

      double phase0 = (mass0.centerOfMassPhase + offset)[0];
      double phase1 = (mass1.centerOfMassPhase + offset)[0];

      phase0 += phase0Adjustment;
      phase1 += phase1Adjustment;

      std::complex<double> complex0 = std::polar(amp0, phase0);
      std::complex<double> complex1 = std::polar(amp1, phase1);

      std::complex<double> complexOut = 
        complex0 * (1 - interpolationFactor) + 
        complex1 * interpolationFactor;

      // Amp out
      long outIndex = centerOfMass + offset;
      if (outIndex >=  0 and outIndex < transformSize) {
        transformOut[outIndex] += complexOut;
      }
    }
  }
}

double Transport::unwrap(
    double phase,
    double previousPhase) {
  double desiredDifference = 
    std::fmod(phase - previousPhase + M_PI, 2 * M_PI);
  if (desiredDifference < 0) desiredDifference += 2 * M_PI;
  return desiredDifference + previousPhase - M_PI;
}

// Rotate phase in the same direction...
double Transport::rotate(
    double phase0,
    double phase1,
    double interpolationFactor) {
  phase0 = std::fmod(phase0,2*M_PI);
  phase1 = std::fmod(phase1,2*M_PI);

  if (phase1 < phase0) {
    phase1 += 2 * M_PI;
  }

  return
    phase0 + interpolationFactor * (phase1 - phase0);
}

#endif

// Seperate complex transform into masses and angles

// Compute the group delay derivative

// Use group delay derivative to segment mass into masschunks

// compute optimal transport on the chunks

// For assignment
// compute new bin given centers of mass and stuff
// (start bin typa deal)
// multiply by assigmnet/mass

// for i in max size centered around both
// add 1 to masses 
// if one doesn't exist, use mass 1, 
// and phase = phase of first thing (should be accumulated)
// subtract 1 after multiplication.
// add to the correct bin

// output!


    //for (int i = -mass0.leftLength; i < mass0.rightLength; i++) {
      //int index = centerOfMass + i;

      //if (index < 0 or index > transformSize) continue;

      //double amp0 = scale0 * (mass0.centerOfMassAmp + i)[0];
      //double phase0 = (mass0.centerOfMassPhase + i)[0];
      //phase0 += phase0Adjustment;

      //transformOut[index] += std::polar(amp0, phase0) * (1 - interpolationFactor);
    //}
