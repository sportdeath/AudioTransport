#ifndef ISTFT_HPP
#define ISTFT_HPP

#include <fftw3.h>

#include <cstddef>

class ISTFT {
  private:
    unsigned int hopSize;
    unsigned int windowSize;

    double * synthesisWindow;
    double * inverseBuffer;
    double * audioBuffer;

    fftw_plan fftwPlan;
    fftw_complex * transform;
    std::size_t transformBytes;
  
  public:
    ISTFT(unsigned int hopSize, unsigned int windowSize);
    ~ISTFT();

    void processTransform(const fftw_complex * transform);

    double * getAudio();
};

#endif
