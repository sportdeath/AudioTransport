#include <complex>
#include <cstddef>

#include <AudioUtilities/CoordinateTransforms.hpp>
#include "Vocoder/PhaseVocoder.hpp"
#include "Vocoder/TransportVocoder.hpp"
#include "Vocoder/Transport.hpp"

TransportVocoder::TransportVocoder(
    unsigned int hopSize, 
    unsigned int windowSize) : 
  PhaseVocoder(hopSize, windowSize)
{
  amplitudes0 = new double[transformSize];
  amplitudes1 = new double[transformSize];
  amplitudesOut = new double[transformSize];
  phases0 = new double[transformSize];
  phases1 = new double[transformSize];

  int maxTransportSize = 2*transformSize - 1;

  assignmentIndices0 = new std::size_t[maxTransportSize];
  assignmentIndices1 = new std::size_t[maxTransportSize];
  assignmentMasses = new double[maxTransportSize];
};

TransportVocoder::~TransportVocoder() {
  delete [] amplitudes0;
  delete [] amplitudes1;
  delete [] amplitudesOut;
  delete [] phases0;
  delete [] phases1;
  delete [] assignmentIndices0;
  delete [] assignmentIndices1;
  delete [] assignmentMasses;
}

void TransportVocoder::processFrameTransform(
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

    // Normalize masses
    double volume0 = 
      Transport::normalize<double>(
        amplitudes0, 
        transformSize,
        amplitudes0
        );
    double volume1 = 
      Transport::normalize<double>(
        amplitudes1, 
        transformSize,
        amplitudes1
        );

    // Compute transformation matrix
    std::size_t numAssignments = 
      Transport::massAssignment<double>(
        amplitudes0,
        amplitudes1,
        transformSize,
        transformSize,
        assignmentIndices0,
        assignmentIndices1,
        assignmentMasses
        );

    // Move mass along transformation matrix
    // According to interpolation factor
    Transport::transport<double>(
        amplitudes0,
        amplitudes1,
        assignmentIndices0,
        assignmentIndices1,
        assignmentMasses,
        numAssignments,
        interpolationFactor,
        amplitudesOut,
        transformSize
        );

    // Unnormalize
    Transport::unnormalize(
        amplitudesOut,
        transformSize,
        volume0,
        volume1,
        interpolationFactor);

    // Convert back to cartesian
    // Using the phase from 0
    CoordinateTransforms::polarToCartesian(
        amplitudesOut,
        phases0,
        transformOut,
        transformSize
        );
}
