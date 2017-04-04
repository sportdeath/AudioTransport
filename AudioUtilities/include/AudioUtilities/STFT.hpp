#ifndef STFT_HPP
#define STFT_HPP

#include <fftw3.h>

class STFT {
  private:
    unsigned int hopSize;
    unsigned int windowSize;

    double * analysisWindow;
    double * audioBuffer;
    double * windowedAudioBuffer;

    fftw_plan fftwPlan;
    fftw_complex * transform;

  public:
    STFT(unsigned int hopSize, unsigned int windowSize);
    ~STFT();

    void processHop(const double * audioHop);
    fftw_complex * getTransform();
};

#endif
