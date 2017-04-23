#ifndef TRANSPORT_VOCODER
#define TRANSPORT_VOCODER

#include <complex>

#include <Vocoder/PhaseVocoder.hpp>

struct SpectralMass {
  std::size_t startIndex;
  std::size_t length;
  double mass;
  double centerOfMass; // relative to start bin
}

class TransportVocoder : public PhaseVocoder {

  private:
    // The transforms
    std::complex<double> *** transforms;

    // Absolute values
    double * amplitudes[2];
    // Complex values divided by absolute values
    std::complex<double> * normals[2];
    // The sum of everything
    double volume[2];

    // The derivative
    double * groupDelayDerivative[2];

    // Masses that encapsulate whole sinusoids
    // Optimal transport is done on these
    SpectralMass * masses[2];

    // Assignments from masses0 to masses1
    // Each assignment has an index into masses0
    // and an index into masses1
    // Plus an assigned mass that they share
    int * assignmentIndices[2];
    double * assignmentMasses;
    // This is the number of assignments
    // its bounded at 2 * (|transport| - 1)
    int numAssignments;

    bool updateMass(int & index, double & mass, double * masses);

    /**
     * Determines a mass assignment for
     * the values in masses0 and masses1
     */
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
