#include <complex>

#include <fftw3.h>

#include <Vocoder/PhaseVocoder.hpp>
#include <Vocoder/BidirectionalVocoder.hpp>

BidirectionalVocoder::BidirectionalVocoder(
    unsigned int hopSize, 
    unsigned int windowSize) : 
  PhaseVocoder(hopSize, windowSize)
{};

void BidirectionalVocoder::processFrameTransform(
    const fftw_complex * transform0,
    const fftw_complex * transform1,
    double interpolationFactor, 
    fftw_complex * transformOut
    ) {

  std::complex<double> complexExponential0;
  std::complex<double> complexExponential1;
  std::complex<double> complexExponentialOut;

  for (int i = 0; i < transformSize; i++) {
    complexExponential0 = 
      std::complex<double>(transform0[i][0], transform0[i][1]);
    complexExponential1 = 
      std::complex<double>(transform1[i][0], transform1[i][1]);

    complexExponentialOut = 
      std::pow(complexExponential0, 1 - interpolationFactor) *
      std::pow(complexExponential1, interpolationFactor);

    transformOut[i][0] = complexExponentialOut.real();
    transformOut[i][1] = complexExponentialOut.imag();
  }
}
