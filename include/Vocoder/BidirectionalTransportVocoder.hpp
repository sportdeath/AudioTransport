#ifndef BIDIRECTIONAL_TRANSPORT_VOCODER
#define BIDIRECTIONAL_TRANSPORT_VOCODER

#include <complex>
#include <cstddef>

#include "Vocoder/SpectralMass.hpp"
#include "Vocoder/PhaseVocoder.hpp"

class BidirectionalTransportVocoder : public PhaseVocoder {
  private:
    int sampleRate;
    int windowSize;
    int hopSize;
    double freqThreshold;

    double * amplitudes0;
    double * amplitudes1;
    double * phases0;
    double * phases1;
    double * accumulatedPhases0;
    double * accumulatedPhases1;

    SpectralMass * masses0;
    SpectralMass * masses1;
    SpectralMass * massesOut;

    double * massesAmpData;
    double * massesPhaseData;

    std::size_t * assignmentIndices0;
    std::size_t * assignmentIndices1;
    double * assignmentMasses;

    void unsegment(
        const SpectralMass * masses,
        std::size_t numAssignments,
        std::complex<double> * transform
        );

    std::size_t segmentIntoMasses(
        std::complex<double> ** transforms,
        double * amplitudes,
        double * accumulatedPhases,
        SpectralMass * masses
        );

    double getBinFrequency(
        std::size_t index
        );

    double getFrequencyAdjustment(
        std::complex<double> ** transforms,
        std::size_t i
        );

    void plotSegmentation(
        std::complex<double> ** transforms,
        SpectralMass * masses,
        std::size_t numMasses);

    void accumulatePhases(
        const double * phases,
        std::complex<double> ** transforms,
        double * accumulatedPhases
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
