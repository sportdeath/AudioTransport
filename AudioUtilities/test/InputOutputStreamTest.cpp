#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>

int main() {
  int framesPerBuffer = 1024;
  int sampleRate = 44100;
  int numOutputChannels = 2;
  int numInputChannels = 4;

  int inputDeviceNum;
  int outputDeviceNum;

  chooseDevices(
      numInputChannels,
      numOutputChannels,
      sampleRate,
      inputDeviceNum,
      outputDeviceNum
      );

  OutputStream outputStream(
      numOutputChannels, 
      framesPerBuffer, 
      sampleRate,
      outputDeviceNum
      );

  double outputLeft[framesPerBuffer];
  double outputRight[framesPerBuffer];
  double * output[2] = {outputLeft, outputRight};

  // make handler function
  std::function<HandleInputBufferFunction> handler =
    [&outputStream, &output, framesPerBuffer](
        double ** audio
      ) {

    // Combine channels 1-2 and 3-4
    for (int i = 0; i < framesPerBuffer; i++) {
      output[0][i] = (audio[0][i] + audio[2][i])/2.;
      output[1][i] = (audio[1][i] + audio[3][i])/2.;
    }
    outputStream.write(output);
  };

  // open stream
  InputStream inputStream(
      &handler, 
      numInputChannels, 
      framesPerBuffer, 
      sampleRate,
      inputDeviceNum
      );

  //
  int numSeconds = 10;
  int millisecondsToSleep = numSeconds*1000;
  std::cout 
    << "Sleeping for " 
    << numSeconds 
    << " seconds." 
    << std::endl;
  std::this_thread::sleep_for(
      std::chrono::milliseconds(millisecondsToSleep)
      );

  std::cout << "Done!" << std::endl;
}
