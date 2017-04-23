#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>
#include <AudioUtilities/ChooseMIDIDevices.hpp>
#include <AudioUtilities/MIDIStream.hpp>

#include <Vocoder/BidirectionalVocoder.hpp>

int main() {
  int hopSize = 1024;
  int windowSize = 4096;
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

  int FADER_VALUE = 64;
  int LINEAR_VOLUME = 48;
  int VOCODER_VOLUME = 49;
  int midiDevice = chooseMIDIDevices();
  std::vector<int> midiValues = {FADER_VALUE, LINEAR_VOLUME, VOCODER_VOLUME};
  MIDIStream midiStream = MIDIStream(midiDevice, midiValues);

  OutputStream outputStream(
      numOutputChannels, 
      hopSize,
      sampleRate,
      outputDeviceNum
      );

  double * output[2];

  BidirectionalVocoder leftVocoder(hopSize, windowSize);
  BidirectionalVocoder rightVocoder(hopSize, windowSize);

  // make handler function
  std::function<HandleInputBufferFunction> handler =
    [
    &midiStream,
    &FADER_VALUE,
    &LINEAR_VOLUME,
    &VOCODER_VOLUME,
    &hopSize,
    &outputStream, 
    &output,
    &leftVocoder,
    &rightVocoder
      ](double ** audio) {

    midiStream.updateValues();
    double interpolationFactor = midiStream.getValue(FADER_VALUE);
    double linearVolume = midiStream.getValue(LINEAR_VOLUME);
    double vocoderVolume = midiStream.getValue(VOCODER_VOLUME);

    // Vocode
    leftVocoder.processFrame(
        audio[0], 
        audio[2], 
        interpolationFactor,
        output[0]
        );
    rightVocoder.processFrame(
        audio[1], 
        audio[3], 
        interpolationFactor,
        output[1]
        );

    for (int channel = 0; channel < 2; channel ++) {
      for (int i = 0; i < hopSize; i++) {
        output[0][i] = 
          output[0][i] * vocoderVolume + 
          (audio[0][i] * (1 - interpolationFactor) + audio[2][i] *interpolationFactor) * linearVolume;
        output[1][i] = 
          output[1][i] * vocoderVolume + 
          (audio[1][i] * (1 - interpolationFactor) + audio[3][i] *interpolationFactor) * linearVolume;
      }
    }

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

  // Wait for user to quit
  std::string userInput;
  std::cout << std::endl;
  while (userInput != "quit") {
    std::cout << "To quit type 'quit': ";
    std::cin >> userInput;
  }
}
