#include <cassert>

#include <fftw3.h>

#include <AudioUtilities/STFT.hpp>
#include <AudioUtilities/Window.hpp>

STFT::STFT(unsigned int hopSize_, unsigned int windowSize_) :
  hopSize(hopSize_), 
  windowSize(windowSize_)
{
  assert(windowSize >= hopSize);

  // Initialize the arrays
  analysisWindow = new double[windowSize];
  audioBuffer = new double[windowSize];
  windowedAudioBuffer = new double[windowSize];
  transform = new fftw_complex[windowSize/2 + 1];

  // Calculate a window
  Window::rootHannWindow(analysisWindow, windowSize);
  
  // Clear the audio buffer
  for (int i = 0; i < windowSize; i++) {
    audioBuffer[i] = 0;
  }

  // Make a plan!
  fftwPlan = fftw_plan_dft_r2c_1d(
      windowSize,
      windowedAudioBuffer,
      transform,
      FFTW_PATIENT | FFTW_DESTROY_INPUT
      );
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
  fftw_execute(fftwPlan);
}

fftw_complex * STFT::getTransform() {
  return transform;
}

STFT::~STFT() {
  fftw_destroy_plan(fftwPlan);
  delete [] analysisWindow;
  delete [] audioBuffer;
  delete [] windowedAudioBuffer;
  delete [] transform;
}
