#include <iostream>

#include <AudioUtilities/ChooseDevices.hpp>

int main() {
  int numInputChannels = 4;
  int numOutputChannels = 2;
  int sampleRate = 44100;

  int inputDevice;
  int outputDevice;

  chooseDevices(
      numInputChannels, 
      numOutputChannels, 
      sampleRate,
      inputDevice,
      outputDevice
      );

  std::cout << "Chose input device : " << inputDevice << std::endl;
  std::cout << "Chose output device : " << outputDevice << std::endl;
}
