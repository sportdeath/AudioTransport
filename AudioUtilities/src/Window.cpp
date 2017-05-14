#include <cmath>

#include "AudioUtilities/Window.hpp"

int Window::oddWindow(double * window, int windowSize) {
  if (windowSize % 2 == 0) {
    // if window is even set the last sample to zero
    window[windowSize - 1] = 0;
    windowSize -= 1;
  }
  return windowSize;
}

void Window::rootHannWindow(double * window, int windowSize) {
  windowSize = oddWindow(window, windowSize);

  int halfWindow = (windowSize - 1)/2;

  for (int i = 0; i < windowSize; i++) {
    // Center the window
    int windowIndex = i - halfWindow;
    window[i] = std::cos(M_PI * windowIndex/double(windowSize));

    // TODO: delete
    //window[i] *= window[i];
  }
}

void Window::rootHannWindowD(double * window, int windowSize, int sampleRate) {
  int oddWindowSize = oddWindow(window, windowSize);
  int halfWindow = (oddWindowSize - 1)/2;

  for (int i = 0; i < windowSize; i++) {
    // Center the window
    int windowIndex = i - halfWindow;
    window[i] = -(M_PI * sampleRate)/double(windowSize) * std::sin(M_PI * windowIndex/double(windowSize));

    // TODO: delete
    // window[i] = -(M_PI * sampleRate)/double(windowSize) * std::sin(2 * M_PI * windowIndex/double(windowSize));
  }

}
