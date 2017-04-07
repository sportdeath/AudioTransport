#include <complex>

#include <Vocoder/PhaseVocoder.hpp>
#include <Vocoder/BidirectionalVocoder.hpp>

BidirectionalVocoder::BidirectionalVocoder(
    unsigned int hopSize, 
    unsigned int windowSize) : 
  PhaseVocoder(hopSize, windowSize)
{};

void BidirectionalVocoder::processFrameTransform(
    std::complex<double> ** transforms0,
    std::complex<double> ** transforms1,
    double interpolationFactor, 
    std::complex<double> * transformOut
    ) {

  for (int i = 0; i < transformSize; i++) {
    transformOut[i] =
      std::pow(transforms0[0][i], 1 - interpolationFactor) *
      std::pow(transforms1[0][i], interpolationFactor);
  }
}
