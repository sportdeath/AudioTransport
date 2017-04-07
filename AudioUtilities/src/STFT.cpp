#include <cassert>

#include <fftw3.h>

#include <AudioUtilities/STFT.hpp>
#include <AudioUtilities/Window.hpp>

STFT::STFT(
    unsigned int hopSize_, 
    unsigned int windowSize_, 
    unsigned int maxTransformDegree_) :
  hopSize(hopSize_), 
  windowSize(windowSize_),
  maxTransformDegree(maxTransformDegree_)
{
  assert(windowSize >= hopSize);

  // Initialize the arrays
  analysisWindow = new double[windowSize];
  audioBuffer = new double[windowSize];
  windowedAudioBuffer = new double[windowSize];

  // Calculate a window
  Window::rootHannWindow(analysisWindow, windowSize);
  
  // Clear the audio buffer
  for (int i = 0; i < windowSize; i++) {
    audioBuffer[i] = 0;
  }

  transforms = new fftw_complex*[maxTransformDegree+1];
  fftwPlans = new fftw_plan[maxTransformDegree+1]; 
  for (int degree = 0; degree <= maxTransformDegree; degree++) {
    // Allocate fourier
    transforms[degree] = new fftw_complex[windowSize/2 + 1];

    // Make a plan!
    fftwPlans[degree] = fftw_plan_dft_r2c_1d(
        windowSize,
        windowedAudioBuffer,
        transforms[degree],
        FFTW_PATIENT | FFTW_DESTROY_INPUT
        );
  }
}

void STFT::processHop(const double * audioHop) {
  // Shift everything in the hop over
  for (int i = 0; i < windowSize - hopSize; i++) {
    audioBuffer[i] = audioBuffer[i+hopSize];
  }

  // Copy hop to end
  for (int i = 0; i < hopSize; i++) {
    audioBuffer[windowSize - hopSize + i] = audioHop[i];
  }

  // Window the audio
  for (int i = 0; i < windowSize; i++) {
    windowedAudioBuffer[i] = audioBuffer[i] * analysisWindow[i];
  }

  // Perform Fourier transform
  for (int degree = 0; degree <= maxTransformDegree; degree++) {
    if (degree > 0) {
      for (int i = 0; i < windowSize; i++) {
        // Take derivatives
        windowedAudioBuffer[i] *= i - windowSize/2;
      }
    }
    fftw_execute(fftwPlans[degree]);
  }
}

fftw_complex * STFT::getTransform(unsigned int degree) {
  return transforms[degree];
}

fftw_complex ** STFT::getTransforms() {
  return transforms;
}

STFT::~STFT() {
  delete [] analysisWindow;
  delete [] audioBuffer;
  delete [] windowedAudioBuffer;
  for (int degree = 0; degree <= maxTransformDegree; degree++) {
    fftw_destroy_plan(fftwPlans[degree]);
    delete [] transforms[degree];
  }
  delete [] fftwPlans;
  delete [] transforms;
}
