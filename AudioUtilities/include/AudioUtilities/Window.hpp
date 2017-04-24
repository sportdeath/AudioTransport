#ifndef WINDOW_HPP
#define WINDOW_HPP

class Window {
  public:
    static int oddWindow(double * window, int windowSize);
    static void rootHannWindow(double * window, int windowSize);
    static void rootHannWindowD(double * window, int windowSize, int sampleRate);
};

#endif
