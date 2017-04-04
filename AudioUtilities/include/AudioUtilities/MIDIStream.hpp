#ifndef MIDI_STREAM_HPP
#define MIDI_STREAM_HPP

#include <portmidi.h>

class MIDIStream {
  private:
    std::map<int:int> controllerValueMap;
    PortMidiStream * portMidiStream;
    PmEvent * buffer

  public:
    fetchNewData() {
      Pm_Read(portMidiStream, buffer,)
    }

    int getControllerValue(int controllerIndex);


    MIDIStream(
        int deviceNum,
        ) {

      Pm_OpenInput(
          &portMidiStream,
          deviceNum,
          NULL, // No special driver info
          bufferSize,
          NULL,
          NULL
          );


    }

    ~MIDIStream() {
      Pm_Close();
    }
        


        std::function<HandleInputBufferFunction> * handleBuffer,
        int numChannels,
        int framesPerBuffer,
        int sampleRate,
        int deviceNum
        );

    ~InputStream();
};

#endif
