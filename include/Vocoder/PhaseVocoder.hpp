#ifndef PHASE_VOCODER_HPP
#define PHASE_VOCODER_HPP

#include <fftw3.h>

#include <AudioUtilities/STFT.hpp>
#include <AudioUtilities/ISTFT.hpp>

class PhaseVocoder {
  private:
    STFT stft0;
    STFT stft1;

    ISTFT istft;

    fftw_complex * transformOut;

  protected:
    unsigned int transformSize;

  public:
    PhaseVocoder(unsigned int hopSize, unsigned int windowSize);

    ~PhaseVocoder();

    void processFrame(
        const double * input0, 
        const double * input1, 
        double interpolationFactor,
        double * & output
        );

    virtual void processFrameTransform(
        const fftw_complex * transform0,
        const fftw_complex * transform1,
        double interpolationFactor, 
        fftw_complex * 
        ) = 0;
};

#endif
