#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
//#define _USE_MATH_DEFINES

#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>

int main() {
  // open stream
  int numInputChannels = 2;
  int numOutputChannels = 2;
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

  OutputStream stream(
      numOutputChannels, 
      framesPerBuffer, 
      sampleRate,
      outputDeviceNum
      );

  double leftChannel[framesPerBuffer];
  double rightChannel[framesPerBuffer];
  double * audio[2] = {leftChannel, rightChannel};

  int leftFreq = 600;
  int rightFreq = leftFreq/2.;
  int numSeconds = 3;

  // Play a tone in the left speaker
  for (int i = 0; i < (sampleRate * numSeconds)/framesPerBuffer; i++) {
    for (int j = 0; j < framesPerBuffer; j++) {
      // Write a sine wave
      double seconds = (i*framesPerBuffer + j)/double(sampleRate);
      leftChannel[j] = std::sin(2*M_PI*leftFreq * seconds);
      rightChannel[j] = 0;
    }
    stream.write(audio);
  }

  // Play a tone in the right speaker
  for (int i = 0; i < (sampleRate * numSeconds)/framesPerBuffer; i++) {
    for (int j = 0; j < framesPerBuffer; j++) {
      // Write a sine wave
      double seconds = (i*framesPerBuffer + j)/double(sampleRate);
      leftChannel[j] = 0;
      rightChannel[j] = std::sin(2*M_PI*rightFreq*seconds);
    }
    stream.write(audio);
  }
}
