#include <iostream>

#include <complex>

#include <fftw3.h>

#include "Vocoder/PhaseVocoder.hpp"
#include "Vocoder/TransportVocoder.hpp"

TransportVocoder::TransportVocoder(
   unsigned int hopSize, 
   unsigned int windowSize
   ) : 
  PhaseVocoder(hopSize, windowSize)
{
  masses0 = new double[transformSize];
  masses1 = new double[transformSize];
  normals0 = new std::complex<double>[transformSize];
  normals1 = new std::complex<double>[transformSize];

  // This is maximum possible number of assignments
  // in the transform
  int maxTransportSize = 2*transformSize - 1;

  assignmentIndices0 = new int[maxTransportSize];
  assignmentIndices1 = new int[maxTransportSize];
  assignmentMasses = new double[maxTransportSize];
}

TransportVocoder::~TransportVocoder() {
  delete [] masses0;
  delete [] masses1;
  delete [] normals0;
  delete [] normals1;
  delete [] assignmentIndices0;
  delete [] assignmentIndices1;
  delete [] assignmentMasses;
}

void TransportVocoder::processFrameTransform(
    std::complex<double> *** transforms_,
    double interpolationFactor, 
    std::complex<double> * transformOut
    ) {

  transforms = transforms_;

  for (std::size_t input = 0; input < NUM_INPUTS; input++) {
    convertToPolar(input)

    // Normalize amplitudes
    normalizeAmplitudes(input);

    // Compute group delay derivative
    computeGroupDelayDerivative(input);

    // segment masses
    segementIntoMasses(input);
  }

  // Determine mass assignment
  determineMassAssignment();

  // Use the mass assignments to
  // create a new output
  processAssignments();
}

void TransportVocoder::convertToPolar(
    std::size_t input
    ) {
  for (int i = 0; i < transformSize; i++) {
    // Compute absolute values
    amplitudes[input][i] = std::abs(transforms[input][0][i]);

    // Compute normals
    normals[input][i] = transforms[input][0][i]/amplitudes[input][i];
  }
}

void TransportVocoder::normalizeAmplitudes(std::size_t input) {
  volume[input] = 0;

  for (int i = 0; i < transformSize; i++) {
    // Accumulate volume
    volume[input] += amplitudes[input][i];
  }

  // Normalize the masses
  for (int i = 0; i < transformSize; i++) {
    amplitudes[input][i] = amplitudes[input][i]/volume[input];
  }
}

void TransportVocoder::computeGroupDelay(std::size_t input) {
  for (int i = 0; i < transformSize; i++) {
    groupDelayDerivative[input][i] = 
      (transforms[input][2][i]/transforms[input][0][i] -
      std::pow(transforms[input][1][i]/transforms[input][0][i], 2)).imag();
  }
}

void TransportVocoder::segementIntoMasses(std::size_t input) {
  currentNonZero = 
  for (int i = 0; i < transformSize; i++) {
    current = transforms[input][0][i];
    if (i + 1 < transformSize)
    while (current) {
      index ++ 
    }
  }
}

void TransportVocoder::determineMassAssignment() {
  numAssignments = 0;

  int index0 = 0;
  int index1 = 0;
  double mass0 = masses[0][index0].mass;
  double mass1 = masses[1][index1].mass;

  while (true) {
    // save the indexes
    assignmentIndices[0][numAssignments] = index0;
    assignmentIndices[1][numAssignments] = index1;

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
    if (not updateMass(index0, mass0, masses0)) break;
    if (not updateMass(index1, mass1, masses1)) break;
  }
}

bool TransportVocoder::updateMass(int & index, double & mass, SpectralMass * masses) {
  while (mass <= 0) {
    index++;
    if (index < transformSize) {
      mass = masses[index].mass;
      return true;
    } else {
      return false;
    }
  } 
  return true;
}

void TransportVocoder::processAssignments() {
  // Fill new output
  for (int i = 0; i < numAssignments; i++) {
    int index0 = assignmentIndices0[i];
    int index1 = assignmentIndices1[i];

    // Raise the normals to a power
    // This interpolates the phase
    std::complex<double> outputNormal = 
      std::pow(normals0[index0], 1 - interpolationFactor) *
      std::pow(normals1[index1], interpolationFactor);

    // Determine output volume
    double outputVolume =
      std::pow(volume0, 1 - interpolationFactor) *
      std::pow(volume1, interpolationFactor);

    // Multiply by the desired mass
    std::complex<double> outputComplex = 
      outputVolume * outputNormal * assignmentMasses[i];

    // The output bin
    double outputIndex = 
      index0 * (1 - interpolationFactor) +
      index1 * interpolationFactor;

    // Linearly interpolate between adjacent bins
    int leftOutputIndex = std::floor(outputIndex);
    int rightOutputIndex = std::floor(outputIndex+1);

    double leftContribution = rightOutputIndex - outputIndex;
    double rightContribution = outputIndex - leftOutputIndex;

    std::complex<double> leftOutputComplex =
      leftContribution * outputComplex;
    std::complex<double> rightOutputComplex =
      rightContribution * outputComplex;

    transformOut[leftOutputIndex][0] += leftOutputComplex.real();
    transformOut[leftOutputIndex][1] += leftOutputComplex.imag();
    transformOut[rightOutputIndex][0] += rightOutputComplex.real();
    transformOut[rightOutputIndex][1] += rightOutputComplex.imag();
  }
}

TransportVocoder::combine
