#include <iostream>
#include <vector>
#include <limits>

#include <portmidi.h>

#include "AudioUtilities/StreamException.hpp"

int chooseMIDIDevices() {
  PmError portMidiError;

  /////////////////////////
  // Initialize port audio
  portMidiError = Pm_Initialize();
  if (portMidiError != pmNoError) {
    std::string errorMessage;
    errorMessage += "Portaudio initialization failed!\n";
    errorMessage += Pm_GetErrorText(portMidiError);
    throw StreamException(errorMessage);
  }
  ////////////////////////

  ////////////////////////
  // List devices
  int numDevices = Pm_CountDevices();
  std::cout << std::endl << std::endl;

  const PmDeviceInfo * deviceInfo;
  std::vector<int> availableDevices;

  for (int deviceNum = 0; deviceNum < numDevices; deviceNum++) {
    deviceInfo = Pm_GetDeviceInfo(deviceNum);

    if (deviceInfo -> input > 0) {
      std::cout 
        << availableDevices.size()
        << ": "
        << deviceInfo -> name
        << std::endl;

      availableDevices.push_back(deviceNum);
    }
  }
  ////////////////////////

  ///////////////////////////
  // Which one would you like?

  int userDeviceSelection = -1;

  std::cout << std::endl;
  std::cout << "Select an input device: ";
  std::cin >> userDeviceSelection;
  while (
      userDeviceSelection < 0 
      or userDeviceSelection >= availableDevices.size()
      or not std::cin
      ) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Select an input device: ";
    std::cin >> userDeviceSelection;
  }
  
  int device = availableDevices[userDeviceSelection];

  std::cout 
    << "Using Device: "
    << Pm_GetDeviceInfo(device) -> name
    << std::endl;
  /////////////////////

  /////////////////////
  // Terminate
  Pm_Terminate();
  ////////////////////

  return device;
}
