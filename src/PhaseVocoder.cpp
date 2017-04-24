#include "Vocoder/PhaseVocoder.hpp"

PhaseVocoder::PhaseVocoder(
    unsigned int hopSize, 
    unsigned int windowSize,
    unsigned int maxTransformDegree_,
    unsigned int sampleRate) :
  maxTransformDegree(maxTransformDegree_),
  stft0(hopSize, windowSize, maxTransformDegree_, sampleRate),
  stft1(hopSize, windowSize, maxTransformDegree_, sampleRate),
  istft(hopSize, windowSize)
{
  transformSize = windowSize/2 + 1;

  transformOut = new std::complex<double>[transformSize];
  transformOutFFTW = new fftw_complex[transformSize];

  transforms0 = new std::complex<double>*[maxTransformDegree+1];
  transforms1 = new std::complex<double>*[maxTransformDegree+1];
  for (int degree = 0; degree <= maxTransformDegree; degree++) {
    transforms0[degree] = new std::complex<double>[transformSize];
    transforms1[degree] = new std::complex<double>[transformSize];
  }
}

PhaseVocoder::~PhaseVocoder() {
  delete [] transformOut;
  delete [] transformOutFFTW;
  for (int degree = 0; degree <= maxTransformDegree; degree++) {
    delete transforms0[degree];
    delete transforms1[degree];
  }
  delete [] transforms0;
  delete [] transforms1;
}

void PhaseVocoder::fftwToComplexTransform(
    const fftw_complex * fftwTransform,
    std::complex<double> * complexTransform) {
  for (int i = 0; i < transformSize; i++) {
    complexTransform[i] = std::complex<double>(fftwTransform[i][0], fftwTransform[i][1]);
  }
}

void PhaseVocoder::complexToFFTWTransform(
    const std::complex<double> * complexTransform,
    fftw_complex * fftwTransform) {
  for (int i = 0; i < transformSize; i++) {
    fftwTransform[i][0] = complexTransform[i].real();
    fftwTransform[i][1] = complexTransform[i].imag();
  }
}

void PhaseVocoder::processFrame(
    const double * input0, 
    const double * input1, 
    double interpolationFactor,
    double * & output
    ) {
    // STFT
    stft0.processHop(input0);
    stft1.processHop(input1);

    fftw_complex ** transforms0FFTW = stft0.getTransforms();
    fftw_complex ** transforms1FFTW = stft1.getTransforms();

    // convert to complex
    for (int degree = 0; degree <= maxTransformDegree; degree++) {
      fftwToComplexTransform(
          transforms0FFTW[degree],
          transforms0[degree]);
      fftwToComplexTransform(
          transforms1FFTW[degree],
          transforms1[degree]);
    }

    // Process with virtual function
    processFrameTransform(
        transforms0,
        transforms1,
        interpolationFactor,
        transformOut
        );

    // convert back to fftw
    complexToFFTWTransform(
        transformOut,
        transformOutFFTW);

    // ISTFT
    istft.processTransform(transformOutFFTW);

    // Return
    output = istft.getAudio();
}
