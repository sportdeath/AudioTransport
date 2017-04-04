#include <vector>
#include <functional>
#include <iostream>

#include <portaudio.h>

#include <AudioUtilities/OutputStream.hpp>
#include <AudioUtilities/StreamException.hpp>

OutputStream::OutputStream(
    int numChannels_,
    int framesPerBuffer_,
    int sampleRate,
    int deviceNum
    ) : 
  framesPerBuffer(framesPerBuffer_),
  numChannels(numChannels_)
{
  PaError portAudioError;

  /////////////////////////
  // Initialize port audio
  portAudioError = Pa_Initialize();
  if (portAudioError != paNoError) {
    // Initialization failed
    std::string errorMessage;
    errorMessage += "Portaudio initialization failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  /////////////////////////

  ///////////////////////////
  // Stream set up 

  // Grab information about the
  // audio device
  const PaDeviceInfo * deviceInfo =
    Pa_GetDeviceInfo(deviceNum);

  // Set the stream parameters
  PaStreamParameters outputParameters;
  outputParameters.device = deviceNum;
  // Only use this many channels
  outputParameters.channelCount = numChannels;
  // use float samples and do not interleave
  outputParameters.sampleFormat = paFloat32;
  // Set to low latency
  outputParameters.suggestedLatency 
    = deviceInfo -> defaultLowOutputLatency;
  // Unneeded
  outputParameters.hostApiSpecificStreamInfo = NULL;

  // No input
  PaStreamParameters * inputParameters = NULL;
  ///////////////////////////

  /////////////////////////
  // Prepare write data
  interleaveBuffer = new float[framesPerBuffer*numChannels];
  /////////////////////////

  /////////////////////////
  // Open The Stream
  portAudioError = Pa_OpenStream(
      &portAudioStream,
      inputParameters,
      &outputParameters,
      sampleRate,
      framesPerBuffer,
      paNoFlag, // Not needed here 
      NULL, // Blocking read write mode
      NULL // Unneeded
      );

  if (portAudioError != paNoError) {
    // Opening the stream failed
    std::string errorMessage;
    errorMessage += "Opening the output stream failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  ///////////////////////////
  
  //////////////////////////
  // Start the stream
  portAudioError = Pa_StartStream(portAudioStream);
  if (portAudioError != paNoError) {
    std::string errorMessage;
    errorMessage += "Starting the output stream failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  ///////////////////////////
}

OutputStream::~OutputStream() {
  // Stop The Stream
  Pa_StopStream(portAudioStream);

  // Close port audio
  Pa_Terminate();

  // remove buffer
  delete interleaveBuffer;
}

void OutputStream::write(double ** audio) const {
  // De interleave audio
  for (int frameNum = 0; frameNum < framesPerBuffer; frameNum++) {
    for (int channelNum = 0; channelNum < numChannels; channelNum++) {
      interleaveBuffer[frameNum*numChannels + channelNum] = 
        audio[channelNum][frameNum];
    }
  }
  Pa_WriteStream(portAudioStream, interleaveBuffer, framesPerBuffer);
}
