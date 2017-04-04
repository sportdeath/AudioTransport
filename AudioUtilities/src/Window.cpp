#include <cmath>

#include <AudioUtilities/Window.hpp>

void Window::rootHannWindow(double * window, int windowSize) {
  if (windowSize % 2 == 0) {
    // if window is even set the last sample to zero
    window[windowSize - 1] = 0;
    windowSize -= 1;
  }

  int halfWindow = (windowSize - 1)/2;

  for (int i = 0; i < windowSize; i++) {
    // Center the window
    int windowIndex = i - halfWindow;
    window[i] = std::cos(M_PI * windowIndex/double(windowSize));
  }
}
