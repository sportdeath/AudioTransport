#include <fftw3.h>

#include <AudioUtilities/CoordinateTransforms.hpp>

#include <Vocoder/PhaseVocoder.hpp>
#include <Vocoder/ClassicVocoder.hpp>

ClassicVocoder::ClassicVocoder(unsigned int hopSize, unsigned int windowSize) : 
  PhaseVocoder(hopSize, windowSize)
{
  amplitudes0 = new double[transformSize];
  amplitudes1 = new double[transformSize];
  amplitudesOut = new double[transformSize];

  phases0 = new double[transformSize];
  phases1 = new double[transformSize];
  phasesOut = new double[transformSize];
}

ClassicVocoder::~ClassicVocoder() {
  delete[] amplitudes0;
  delete[] amplitudes1;
  delete[] amplitudesOut;
  delete[] phases0;
  delete[] phases1;
  delete[] phasesOut;
}

void ClassicVocoder::processFrameTransform(
    const fftw_complex * transform0,
    const fftw_complex * transform1,
    double interpolationFactor, 
    fftw_complex * transformOut
    ) {

  // Convert to amp and phase
  CoordinateTransforms::cartesianToPolar(
      transform0,
      amplitudes0,
      phases0,
      transformSize);
  CoordinateTransforms::cartesianToPolar(
      transform1,
      amplitudes1,
      phases1,
      transformSize);

  for (int i = 0; i < transformSize; i++) {
    // Power of amplitudes
    amplitudesOut[i]  = 
      std::pow(amplitudes0[i], 1-interpolationFactor) *
      std::pow(amplitudes1[i], interpolationFactor);
    // Take phase from first input
    phasesOut[i] = phases0[i];
  }

  // Convert back to complex
  CoordinateTransforms::polarToCartesian(
      amplitudesOut,
      phasesOut,
      transformOut,
      transformSize);
}
