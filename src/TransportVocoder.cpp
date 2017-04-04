#include <iostream>

#include <complex>

#include <fftw3.h>

#include <Vocoder/PhaseVocoder.hpp>
#include <Vocoder/TransportVocoder.hpp>

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

bool TransportVocoder::updateMass(int & index, double & mass, double * masses) {
  while (mass <= 0) {
    index++;
    if (index < transformSize) {
      mass = masses[index];
      return true;
    } else {
      return false;
    }
  } 
  return true;
}

void TransportVocoder::determineMassAssignment() {

  numAssignments = 0;

  int index0 = 0;
  int index1 = 0;
  double mass0 = masses0[index0];
  double mass1 = masses1[index1];

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
    if (not updateMass(index0, mass0, masses0)) break;
    if (not updateMass(index1, mass1, masses1)) break;
  }
}

void TransportVocoder::processFrameTransform(
    const fftw_complex * transform0,
    const fftw_complex * transform1,
    double interpolationFactor, 
    fftw_complex * transformOut
    ) {

  volume0 = 0;
  volume1 = 0;
  
  for (int i = 0; i < transformSize; i++) {
    // Convert to complex format
    std::complex<double> complex0 = 
      std::complex<double>(transform0[i][0], transform0[i][1]);
    std::complex<double> complex1 = 
      std::complex<double>(transform1[i][0], transform1[i][1]);

    // Compute absolute values
    masses0[i] = std::abs(complex0);
    masses1[i] = std::abs(complex1);

    // Accumulate volume
    volume0 += masses0[i];
    volume1 += masses1[i];

    // Compute normals
    normals0[i] = complex0/masses0[i];
    normals1[i] = complex1/masses1[i];

    // Zero the output
    transformOut[i][0] = 0;
    transformOut[i][1] = 0;
  }

  // Normalize the masses
  double massesSum = 0;
  for (int i = 0; i < transformSize; i++) {
    masses0[i] = masses0[i]/volume0;
    masses1[i] = masses1[i]/volume1;
    massesSum += masses0[i];
  }

  // Determine mass assignment
  determineMassAssignment();
  
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
