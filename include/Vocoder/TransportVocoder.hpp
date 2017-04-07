#ifndef TRANSPORT_VOCODER
#define TRANSPORT_VOCODER

#include <complex>

#include <Vocoder/PhaseVocoder.hpp>

class TransportVocoder : public PhaseVocoder {
  private:
    double * masses0;
    double * masses1;
    std::complex<double> * normals0;
    std::complex<double> * normals1;
    double volume0;
    double volume1;

    int * assignmentIndices0;
    int * assignmentIndices1;
    double * assignmentMasses;
    int numAssignments;

    bool updateMass(int & index, double & mass, double * masses);
    void determineMassAssignment();

  public:
    TransportVocoder(unsigned int hopSize, unsigned int windowSize);

    ~TransportVocoder();

    void processFrameTransform(
        const std::complex<double> ** transforms0,
        const std::complex<double> ** transforms1,
        double interpolationFactor, 
        std::complex<double> * transformOut
        );
};

#endif
