#ifndef CLASSIC_VOCODER_HPP
#define CLASSIC_VOCODER_HPP

#include <complex>

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
        std::complex<double> ** transforms0,
        std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        );
};

#endif
