#ifndef PHASE_VOCODER_HPP
#define PHASE_VOCODER_HPP

#include <complex>

#include <fftw3.h>

#include <AudioUtilities/STFT.hpp>
#include <AudioUtilities/ISTFT.hpp>

class PhaseVocoder {
  private:
    STFT stft0;
    STFT stft1;

    ISTFT istft;

    unsigned int maxTransformDegree;

    fftw_complex * transformOutFFTW;

    std::complex<double> ** transforms0;
    std::complex<double> ** transforms1;
    std::complex<double> * transformOut;

    void fftwToComplexTransform(
        const fftw_complex * fftwTransform,
        std::complex<double> * complexTransform);

    void complexToFFTWTransform(
        const std::complex<double> * complexTransform,
        fftw_complex * fftwTransform);

  protected:
    unsigned int transformSize;

  public:
    PhaseVocoder(
        unsigned int hopSize, 
        unsigned int windowSize,
        unsigned int maxTransformDegree = 0,
        unsigned int sampleRate = 1);

    ~PhaseVocoder();

    void processFrame(
        const double * input0, 
        const double * input1, 
        double interpolationFactor,
        double * & output
        );

    virtual void processFrameTransform(
        std::complex<double> ** transforms0,
        std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        ) = 0;
};

#endif
