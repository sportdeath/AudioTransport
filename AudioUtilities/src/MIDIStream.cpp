#include <cstddef>
#include <iostream>
#include <vector>

#include <portmidi.h>
#include <porttime.h>

#include "AudioUtilities/MIDIStream.hpp"
#include "AudioUtilities/StreamException.hpp"

MIDIStream::MIDIStream(
    int deviceNum, 
    std::vector<int> controllerNumbers
    ) {
  PmError portMidiError;

  /////////////////////////
  // Initialize port audio
  portMidiError = Pm_Initialize();
  if (portMidiError != pmNoError) {
    std::string errorMessage;
    errorMessage += "Portmidi initialization failed!\n";
    errorMessage += Pm_GetErrorText(portMidiError);
    throw StreamException(errorMessage);
  }
  /////////////////////////

  /////////////////////////
  // Open a time
  //Pt_Start(1, NULL, NULL);
  //Pt_Stop();

  /////////////////////////
  // Open the stream
  portMidiError = Pm_OpenInput(
      &stream,
      deviceNum,
      NULL,
      BUFFER_SIZE,
      NULL,
      NULL);
  if (portMidiError != pmNoError) {
    std::string errorMessage;
    errorMessage += "Opening MIDI stream failed!\n";
    errorMessage += Pm_GetErrorText(portMidiError);
    throw StreamException(errorMessage);
  }
  /////////////////////////
  
  for (int i = 0; i < controllerNumbers.size(); i++) {
    values[controllerNumbers[i]] = 0;
  }
}

MIDIStream::~MIDIStream() {
  Pm_Terminate();
}

void MIDIStream::updateValues()  {
  // Read the events
  int numRead = Pm_Read(stream, buffer, BUFFER_SIZE);
  PmEvent event;
  // Print the messages
  for (int i = 0; i < numRead; i++) {
    event = buffer[i];
    short status = Pm_MessageStatus(event.message);

    if (status >= 0xB0 and status < 0xC0) {
      short controllerNumber = Pm_MessageData1(event.message);

      // If controller number is in the map
      if (values.find(controllerNumber) != values.end()) {
        values[controllerNumber] = Pm_MessageData2(event.message)/127.;
      }
    }
  }
}

double MIDIStream::getValue(int controllerNumber) {
  return values[controllerNumber];
}
