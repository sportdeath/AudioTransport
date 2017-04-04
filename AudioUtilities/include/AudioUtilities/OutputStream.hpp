#ifndef OUTPUT_STREAM_HPP
#define OUTPUT_STREAM_HPP

#include <functional>

#include <portaudio.h>

class OutputStream {
  private:
    PaStream * portAudioStream;
    int framesPerBuffer;
    int numChannels;
    float * interleaveBuffer;

  public:
    OutputStream(
        int numChannels,
        int framesPerBuffer,
        int sampleRate,
        int deviceNum
        );

    ~OutputStream();

    void write(double ** audio) const;
};

#endif
