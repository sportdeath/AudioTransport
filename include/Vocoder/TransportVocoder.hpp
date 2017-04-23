#ifndef TRANSPORT_VOCODER_HPP
#define TRANSPORT_VOCODER_HPP

#include <complex>
#include <cstddef>

#include <Vocoder/PhaseVocoder.hpp>

class TransportVocoder : public PhaseVocoder {
  private:
    double * amplitudes0;
    double * amplitudes1;
    double * amplitudesOut;
    double * phases0;
    double * phases1;
    std::size_t * assignmentIndices0;
    std::size_t * assignmentIndices1;
    double * assignmentMasses;
  public:
    TransportVocoder(unsigned int hopSize, unsigned int windowSize);

    ~TransportVocoder();

    void processFrameTransform(
        std::complex<double> ** transforms0,
        std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        );
};

#endif
