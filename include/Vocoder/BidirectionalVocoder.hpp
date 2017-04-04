#ifndef BIDIRECTIONAL_VOCODER_HPP
#define BIDIRECTIONAL_VOCODER_HPP

#include <fftw3.h>

#include <Vocoder/PhaseVocoder.hpp>

class BidirectionalVocoder : public PhaseVocoder {
  public:
    BidirectionalVocoder(unsigned int hopSize, unsigned int windowSize);

    void processFrameTransform(
        const fftw_complex * transform0,
        const fftw_complex * transform1,
        double interpolationFactor, 
        fftw_complex * transformOut
        );
};

#endif
