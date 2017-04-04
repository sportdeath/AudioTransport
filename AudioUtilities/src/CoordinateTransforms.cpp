#include <iostream>

#include <cmath>
#include <complex>

#include <fftw3.h>

#include <AudioUtilities/CoordinateTransforms.hpp>

const std::complex<double> CoordinateTransforms::I = 
    std::complex<double>(0,1);

void CoordinateTransforms::cartesianToPolar(
  const fftw_complex * transform,
  double * amplitudes,
  double * phases,
  int transformSize
  ) {
  for (int i = 0; i < transformSize; i++) {
    amplitudes[i] = 
      std::sqrt(
      transform[i][0]*transform[i][0] + 
      transform[i][1]*transform[i][1]
      );

    phases[i] = std::atan2(transform[i][1], transform[i][0]);
  }

  // Phase unwrapping
  double phaseDifference;
  for (int i = 1; i < transformSize; i++) {
    phaseDifference = phases[i] - phases[i-1];

    phases[i] +=
      -2*M_PI*std::floor((phaseDifference + M_PI)/(2*M_PI));
  }
}

void CoordinateTransforms::polarToCartesian(
  const double * amplitudes,
  const double * phases,
  fftw_complex * transform,
  int transformSize
  ) {
  
  std::complex<double> complexExponential;
  for (int i = 0; i < transformSize; i++) {
    complexExponential = amplitudes[i] * std::exp(I * phases[i]);
    transform[i][0] = complexExponential.real();
    transform[i][1] = complexExponential.imag();
  }
}
