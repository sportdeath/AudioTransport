#ifndef INPUT_STREAM_HPP
#define INPUT_STREAM_HPP

#include <functional>

#include <portaudio.h>

typedef void HandleInputBufferFunction(
    double ** audio
    );

struct InputStreamData {
  std::function<HandleInputBufferFunction> * handleBuffer;
  double ** deinterleaveBuffer;
  int numChannels;
};

class InputStream {
  private:
    PaStream * portAudioStream;
    InputStreamData userData;

    static int callback(
        const void * inputBuffer,
        void * outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo * timeInfo,
        PaStreamCallbackFlags statusFlags,
        void * userData
        );

  public:
    InputStream(
        std::function<HandleInputBufferFunction> * handleBuffer,
        int numChannels,
        int framesPerBuffer,
        int sampleRate,
        int deviceNum
        );

    ~InputStream();
};

#endif
