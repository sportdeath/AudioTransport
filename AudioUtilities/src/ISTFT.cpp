#include <cassert>
#include <cstring>
#include <iostream>

#include <fftw3.h>

#include <AudioUtilities/ISTFT.hpp>
#include <AudioUtilities/Window.hpp>

ISTFT::ISTFT(unsigned int hopSize_, unsigned int windowSize_) :
  hopSize(hopSize_), 
  windowSize(windowSize_)
{
  assert(windowSize >= hopSize);

  // Initialize the arrays
  synthesisWindow = new double[windowSize];
  inverseBuffer = new double[windowSize];
  audioBuffer = new double[windowSize];
  transformBytes = sizeof(fftw_complex) * (windowSize/2 + 1);
  transform = new fftw_complex[windowSize/2 + 1];

  // Calculate a window
  Window::rootHannWindow(synthesisWindow, windowSize);
  
  // Clear the audio buffer
  for (int i = 0; i < windowSize; i++) {
    audioBuffer[i] = 0;
  }

  // Make a plan!
  fftwPlan = fftw_plan_dft_c2r_1d(
      windowSize,
      transform,
      inverseBuffer,
      FFTW_PATIENT | FFTW_DESTROY_INPUT
      );
}

void ISTFT::processTransform(const fftw_complex * transform_) {
  // Copy the transform window
  std::memcpy(transform, transform_, transformBytes);

  // Fourier transform
  fftw_execute(fftwPlan);

  // Shift the audio buffer over by the hop size
  for (int i = 0; i < windowSize - hopSize; i++) {
    audioBuffer[i] = audioBuffer[i+hopSize];
  }
  
  // clear the end of the buffer
  for (int i = windowSize - hopSize; i < windowSize; i++) {
    audioBuffer[i] = 0;
  }

  // Window and add the inverse buffer
  // to the overlap add buffer
  // (Weighted overlap add)
  //
  // Also scale by (window size) as per FFTW
  for (int i = 0; i < windowSize; i++) {
    audioBuffer[i] += (synthesisWindow[i] * inverseBuffer[i])
      /double(windowSize);
  }
}

double * ISTFT::getAudio() {
  return audioBuffer;
}

ISTFT::~ISTFT() {
  fftw_destroy_plan(fftwPlan);
  delete [] synthesisWindow;
  delete [] inverseBuffer;
  delete [] audioBuffer;
  delete [] transform;
}
