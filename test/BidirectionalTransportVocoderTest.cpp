#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/ChooseDevices.hpp>
#include <AudioUtilities/ChooseMIDIDevices.hpp>
#include <AudioUtilities/MIDIStream.hpp>

#include <Vocoder/BidirectionalTransportVocoder.hpp>

int main() {
  int hopSize = 4096;
  //int hopSize = 1024;
  //int hopSize = 2048;
  //int hopSize = 512;
  //int windowSize = 2048;
  int windowSize = 4096;
  int sampleRate = 44100;
  int numOutputChannels = 2;
  int numInputChannels = 6;

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

  BidirectionalTransportVocoder leftVocoder(hopSize, windowSize, sampleRate);
  BidirectionalTransportVocoder rightVocoder(hopSize, windowSize, sampleRate);

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
        audio[2], 
        audio[4], 
        interpolationFactor,
        output[0]
        );
    rightVocoder.processFrame(
        audio[3], 
        audio[5], 
        interpolationFactor,
        output[1]
        );

    //for (int channel = 0; channel < 2; channel ++) {
      //for (int i = 0; i < hopSize; i++) {
        //output[0][i] = 
          //output[0][i] * vocoderVolume + 
          //(audio[0][i] * (1 - interpolationFactor) + audio[2][i] *interpolationFactor) * linearVolume +
          //audio[4][i];
        //output[1][i] = 
          //output[1][i] * vocoderVolume + 
          //(audio[1][i] * (1 - interpolationFactor) + audio[3][i] *interpolationFactor) * linearVolume +
          //audio[5][i];
      //}
    //}

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
