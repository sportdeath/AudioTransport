#ifndef SPECTRAL_MASS_HPP
#define SPECTRAL_MASS_HPP

#include <cstddef>

struct SpectralMass {
  std::size_t startIndex;
  std::size_t length;
  double mass;
  double centerOfMass; // relative to start bin
};

#endif
