#ifndef COORDINATE_TRANSFORMS_HPP
#define COORDINATE_TRANSFORMS_HPP

#include <complex>

#include <fftw3.h>

class CoordinateTransforms {
  private:
    static const std::complex<double> I;
  public:
    static void cartesianToPolar(
      const fftw_complex * transform,
      double * amplitudes,
      double * phases,
      int transformSize
      );

    static void polarToCartesian(
      const double * amplitudes,
      const double * phases,
      fftw_complex * transform,
      int transformSize
      );

    static void cartesianToPolar(
      const std::complex<double> * transform,
      double * amplitudes,
      double * phases,
      int transformSize
      );

    static void polarToCartesian(
      const double * amplitudes,
      const double * phases,
      std::complex<double> * transform,
      int transformSize
      );
};

#endif
