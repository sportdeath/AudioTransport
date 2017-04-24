#ifndef BIDIRECTIONAL_TRANSPORT_VOCODER
#define BIDIRECTIONAL_TRANSPORT_VOCODER

#include <complex>
#include <cstddef>

#include "Vocoder/SpectralMass.hpp"
#include "Vocoder/PhaseVocoder.hpp"

class BidirectionalTransportVocoder : public PhaseVocoder {
  private:
    int sampleRate;
    double GROUP_DELAY_D_THRESHOLD = 100000000;
    double * amplitudes0;
    double * amplitudes1;
    double * phases0;
    double * phases1;

    SpectralMass * masses0;
    SpectralMass * masses1;
    SpectralMass * massesOut;

    std::complex<double> * massesData;

    std::size_t * assignmentIndices0;
    std::size_t * assignmentIndices1;
    double * assignmentMasses;

    void unsegment(
        const SpectralMass * masses,
        std::size_t numAssignments,
        std::complex<double> * transform
        );

    std::size_t segementIntoMasses(
        std::complex<double> ** transforms,
        SpectralMass * masses
        );

    SpectralMass getMass(
        std::complex<double> * transform,
        std::size_t startIndex,
        std::size_t endIndex
        );

    double getGroupDelayDerivative(
        std::complex<double> ** transforms,
        std::size_t i);

    double getBinFrequency(
        std::size_t index
        );

    double getReassignedFrequency(
        std::complex<double> ** transforms,
        std::size_t index
        );

  public:
    BidirectionalTransportVocoder(unsigned int hopSize, unsigned int windowSize, unsigned int sampleRate);

    ~BidirectionalTransportVocoder();

    void processFrameTransform(
        std::complex<double> ** transforms0,
        std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        );
};

#endif
