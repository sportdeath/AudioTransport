#ifndef BIDIRECTIONAL_VOCODER_HPP
#define BIDIRECTIONAL_VOCODER_HPP

#include <complex>

#include <Vocoder/PhaseVocoder.hpp>

class BidirectionalVocoder : public PhaseVocoder {
  public:
    BidirectionalVocoder(unsigned int hopSize, unsigned int windowSize);

    void processFrameTransform(
        std::complex<double> ** transforms0,
        std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        );
};

#endif
