#include <iostream>
#include <vector>
#include <limits>

#include <portaudio.h>

#include <AudioUtilities/StreamException.hpp>

void chooseDevices(
    int numInputChannels,
    int numOutputChannels,
    int sampleRate,
    int & inputDevice, 
    int & outputDevice
    ) {
  PaError portAudioError;

  /////////////////////////
  // Initialize port audio
  portAudioError = Pa_Initialize();
  if (portAudioError != paNoError) {
    std::string errorMessage;
    errorMessage += "Portaudio initialization failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  ////////////////////////

  //////////////////////////
  // Fake device parameters

  PaStreamParameters deviceParameters;
  // use float samples and interleave
  deviceParameters.sampleFormat = paFloat32;
  // Unneeded
  deviceParameters.hostApiSpecificStreamInfo = NULL;

  const PaDeviceInfo * deviceInfo;
  int numDevices = Pa_GetDeviceCount();
  int userDeviceSelection;
  //////////////////////////

  ////////////////////////
  // Get input device
  
  std::cout << std::endl << std::endl;

  userDeviceSelection = -1;

  // Only use this many channels
  deviceParameters.channelCount = numInputChannels;

  std::vector<int> availableInputDevices;
  for (int deviceNum = 0; deviceNum < numDevices; deviceNum++) {
    deviceInfo = Pa_GetDeviceInfo(deviceNum);

    // Set to device
    deviceParameters.device = deviceNum;

    // Set to low latency
    deviceParameters.suggestedLatency = 
      deviceInfo -> defaultLowInputLatency;

    portAudioError = 
      Pa_IsFormatSupported(&deviceParameters, NULL, sampleRate);

    if (portAudioError == paFormatIsSupported) {
      std::cout 
        << availableInputDevices.size()
        << ": "
        << deviceInfo -> name
        << std::endl;
      availableInputDevices.push_back(deviceNum);
    }
  }

  // Which one would you like?
  std::cout << std::endl;
  std::cout << "Select an input device: ";
  std::cin >> userDeviceSelection;
  while (
      userDeviceSelection < 0 
      or userDeviceSelection >= availableInputDevices.size()
      or not std::cin
      ) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Select an input device: ";
    std::cin >> userDeviceSelection;
  }
  inputDevice = availableInputDevices[userDeviceSelection];
  std::cout 
    << "Using Device: "
    << Pa_GetDeviceInfo(inputDevice) -> name
    << std::endl;
  ///////////////////////

  ////////////////////////
  // Get output device
  
  std::cout << std::endl << std::endl;

  userDeviceSelection = -1;

  // Only use this many channels
  deviceParameters.channelCount = numOutputChannels;

  std::vector<int> availableOutputDevices;
  for (int deviceNum = 0; deviceNum < numDevices; deviceNum++) {
    deviceInfo = Pa_GetDeviceInfo(deviceNum);

    // Set to device
    deviceParameters.device = deviceNum;

    // Set to low latency
    deviceParameters.suggestedLatency = 
      deviceInfo -> defaultLowOutputLatency;

    portAudioError = 
      Pa_IsFormatSupported(NULL, &deviceParameters, sampleRate);

    if (portAudioError == paFormatIsSupported) {
      std::cout 
        << availableOutputDevices.size()
        << ": "
        << deviceInfo -> name
        << std::endl;
      availableOutputDevices.push_back(deviceNum);
    }
  }

  // Which one would you like?
  std::cout << std::endl;
  std::cout << "Select an input device: ";
  std::cin >> userDeviceSelection;
  while (
      userDeviceSelection < 0 
      or userDeviceSelection >= availableOutputDevices.size()
      or not std::cin
      ) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Select an input device: ";
    std::cin >> userDeviceSelection;
  }
  outputDevice = availableOutputDevices[userDeviceSelection];
  std::cout 
    << "Using Device: "
    << Pa_GetDeviceInfo(outputDevice) -> name
    << std::endl;
  ///////////////////////

  /////////////////////
  // Terminate
  Pa_Terminate();
  ////////////////////
}
