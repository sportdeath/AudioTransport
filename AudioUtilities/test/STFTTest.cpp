#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>
#include <AudioUtilities/STFT.hpp>
#include <AudioUtilities/ISTFT.hpp>

int main() {
  int hopSize = 1024;
  int windowSize = 2048;
  int sampleRate = 44100;
  int numOutputChannels = 2;
  int numInputChannels = 2;

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
      hopSize,
      sampleRate,
      outputDeviceNum
      );

  STFT leftSTFT(hopSize, windowSize);
  STFT rightSTFT(hopSize, windowSize);

  ISTFT leftISTFT(hopSize, windowSize);
  ISTFT rightISTFT(hopSize, windowSize);

  double * output[2] = {leftISTFT.getAudio(), rightISTFT.getAudio()};

  // make handler function
  std::function<HandleInputBufferFunction> handler =
    [
    &outputStream, 
    &leftSTFT,
    &rightSTFT,
    &leftISTFT,
    &rightISTFT,
    &output
      ](double ** audio) {

    leftSTFT.processHop(audio[0]);
    rightSTFT.processHop(audio[1]);

    leftISTFT.processTransform(leftSTFT.getTransform());
    rightISTFT.processTransform(rightSTFT.getTransform());

    outputStream.write(output);
  };

  // open stream
  InputStream inputStream(
      &handler, 
      numInputChannels, 
      hopSize,
      sampleRate,
      inputDeviceNum
      );

  //
  int numSeconds = 100;
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
