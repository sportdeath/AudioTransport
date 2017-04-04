#ifndef CLASSIC_VOCODER_HPP
#define CLASSIC_VOCODER_HPP

#include <fftw3.h>

#include <Vocoder/PhaseVocoder.hpp>

class ClassicVocoder : public PhaseVocoder {
  private:
    double * amplitudes0;
    double * amplitudes1;
    double * amplitudesOut;

    double * phases0;
    double * phases1;
    double * phasesOut;

  public:
    ClassicVocoder(unsigned int hopSize, unsigned int windowSize);

    ~ClassicVocoder();

    void processFrameTransform(
        const fftw_complex * transform0,
        const fftw_complex * transform1,
        double interpolationFactor, 
        fftw_complex * transformOut
        );
};

#endif
