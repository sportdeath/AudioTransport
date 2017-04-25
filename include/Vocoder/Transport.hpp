#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include <cstddef>

/**
 * Class to perform optimal
 * transport on spectral masses
 */
class Transport {
  public:
    /**
     * Updates the mass to the
     * next nonzero mass.
     */
    template <class MassContainer>
    static bool updateMass(
        const MassContainer * masses,
        const std::size_t numMasses,
        std::size_t & index, 
        double & mass
        );

    /**
     * Returns the number of assignments
     * made. Fills assignmentIndices and
     * assignmentMasses.
     */
    template <class MassContainer>
    static std::size_t massAssignment(
        const MassContainer * masses0,
        const MassContainer * masses1,
        const std::size_t numMasses0,
        const std::size_t numMasses1,
        std::size_t * assignmentIndices0,
        std::size_t * assignmentIndices1,
        double * assignmentMasses
        );

    template <class MassContainer>
    static void transport(
        const MassContainer * masses0,
        const MassContainer * masses1,
        const std::size_t * assignmentIndices0,
        const std::size_t * assignmentIndices1,
        const double * assignmentMasses,
        const std::size_t numAssignments,
        const double interpolationFactor,
        MassContainer * massesOut,
        const std::size_t numMasses
        );
    
    template <class MassContainer>
    static double getMass(const MassContainer mass);

    template <class MassContainer>
    static void setMass(
        MassContainer * masses, 
        std::size_t index, 
        double mass);

    template <class MassContainer>
    static double normalize(
        const MassContainer * massesIn, 
        std::size_t numMasses,
        MassContainer * massesOut);

    template <class MassContainer>
    static void unnormalize(
        MassContainer * masses, 
        std::size_t numMasses,
        double volume0, 
        double volume1, 
        double interpolationFactor);

    inline
    static std::complex<double> rotate(
        std::complex<double> value0,
        std::complex<double> value1,
        double interpolationFactor);
};

#include "../../src/Transport.cpp"

#endif

// Seperate complex transform into masses and angles

// Compute the group delay derivative

// Use group delay derivative to segment mass into masschunks

// compute optimal transport on the chunks

// For assignment
// compute new bin given centers of mass and stuff
// (start bin typa deal)
// multiply by assigmnet/mass

// for i in max size centered around both
// add 1 to masses 
// if one doesn't exist, use mass 1, 
// and phase = phase of first thing (should be accumulated)
// subtract 1 after multiplication.
// add to the correct bin

// output!
