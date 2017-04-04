#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>

int main() {
  // make handler function
  std::function<HandleInputBufferFunction> handler =
    [](
        double ** audio
      ) {
    std::cout << "Frame received!" << std::endl;
  };

  // open stream
  int numInputChannels = 4;
  int numOutputChannels = 0;
  int framesPerBuffer = 1024;
  int sampleRate = 44100;

  int inputDeviceNum;
  int outputDeviceNum;

  chooseDevices(
      numInputChannels,
      numOutputChannels,
      sampleRate,
      inputDeviceNum,
      outputDeviceNum
      );

  InputStream stream(
      &handler, 
      numInputChannels, 
      framesPerBuffer, 
      sampleRate,
      inputDeviceNum
      );

  int numFramesToSleep = 10;
  int millisecondsToSleep = 
    (1000 * numFramesToSleep * framesPerBuffer)
    /sampleRate;
  std::cout 
    << "Sleeping for " 
    << millisecondsToSleep 
    << " milliseconds" 
    << std::endl;
  std::this_thread::sleep_for(
      std::chrono::milliseconds(3*millisecondsToSleep)
      );
}
