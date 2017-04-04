#include <vector>
#include <functional>
#include <iostream>

#include <portaudio.h>

#include <AudioUtilities/InputStream.hpp>
#include <AudioUtilities/StreamException.hpp>

int InputStream::callback(
    const void * input,
    void * output,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo * timeInfo,
    PaStreamCallbackFlags statusFlags,
    void * userData
    ) {

  // Cast input
  float * inputBuffer = (float *) input;

  // Get User Data
  InputStreamData * data = (InputStreamData *) userData;
  std::function<HandleInputBufferFunction> * handleBuffer = 
    data -> handleBuffer;
  double ** deinterleaveBuffer = data -> deinterleaveBuffer;
  int numChannels = data -> numChannels;

  // De-interleave
  for (int frameNum = 0; frameNum < framesPerBuffer; frameNum++) {
    for (int channel = 0; channel < numChannels; channel++) {
      deinterleaveBuffer[channel][frameNum] =
        inputBuffer[frameNum * numChannels + channel];
    }
  }

  // Process
  (*handleBuffer)(deinterleaveBuffer);

  return 0;
}

// Port audio must already be initialized
InputStream::InputStream(
    std::function<HandleInputBufferFunction> * handleBuffer,
    int numChannels,
    int framesPerBuffer,
    int sampleRate,
    int deviceNum
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

  ///////////////////////////
  // Stream set up 

  // Grab information about the
  // audio device
  const PaDeviceInfo * deviceInfo =
    Pa_GetDeviceInfo(deviceNum);

  PaStreamParameters inputParameters;
  inputParameters.device = deviceNum;
  // Only use this many channels
  inputParameters.channelCount = numChannels;
  // use float samples and interleave
  inputParameters.sampleFormat = paFloat32;
  // Set to low latency
  inputParameters.suggestedLatency = 
    deviceInfo -> defaultLowInputLatency;
  // Unneeded
  inputParameters.hostApiSpecificStreamInfo = NULL;

  // No Output
  PaStreamParameters * outputParameters = NULL;
  ///////////////////////////
  
  //////////////////////////
  // Prepare User Data

  double ** deinterleaveBuffer = new double*[numChannels];
  for (int channel = 0; channel < numChannels; channel++) {
    deinterleaveBuffer[channel] = new double[framesPerBuffer];
  }

  userData.handleBuffer = handleBuffer;
  userData.deinterleaveBuffer = deinterleaveBuffer;
  userData.numChannels = numChannels;
  /////////////////////////

  ////////////////////////
  // Open The Stream
  portAudioError = Pa_OpenStream(
      &portAudioStream,
      &inputParameters,
      outputParameters,
      sampleRate,
      framesPerBuffer, // Frames per buffer
      paNoFlag, // Not needed here 
      callback,
      &userData // No user data
      );

  if (portAudioError != paNoError) {
    std::string errorMessage;
    errorMessage += "Opening the input stream failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  //////////////////////////
  
  /////////////////////////
  // Start the stream
  portAudioError = Pa_StartStream(portAudioStream);
  if (portAudioError != paNoError) {
    std::string errorMessage;
    errorMessage += "Starting the input stream failed!\n";
    errorMessage += Pa_GetErrorText(portAudioError);
    throw StreamException(errorMessage);
  }
  /////////////////////////
}

InputStream::~InputStream() {
  // Stop The Stream
  Pa_StopStream(portAudioStream);

  // Close port audio
  Pa_Terminate();

  // Remove buffer
  delete userData.deinterleaveBuffer;
}
