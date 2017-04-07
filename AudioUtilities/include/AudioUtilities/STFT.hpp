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

    unsigned int maxTransformDegree;
    fftw_plan * fftwPlans;
    fftw_complex ** transforms;

  public:
    STFT(
        unsigned int hopSize, 
        unsigned int windowSize, 
        unsigned int maxTransformDegree = 0);
    ~STFT();

    void processHop(const double * audioHop);
    fftw_complex * getTransform(unsigned int transformDegree = 0);
    fftw_complex ** getTransforms();
};

#endif
