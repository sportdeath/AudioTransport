#include <Vocoder/PhaseVocoder.hpp>

PhaseVocoder::PhaseVocoder(unsigned int hopSize, unsigned int windowSize) :
  stft0(hopSize, windowSize),
  stft1(hopSize, windowSize),
  istft(hopSize, windowSize)
{
  transformSize = windowSize/2 + 1;

  transformOut = new fftw_complex[transformSize]; 
}

PhaseVocoder::~PhaseVocoder() {
  delete [] transformOut;
}

void PhaseVocoder::processFrame(
    const double * input0, 
    const double * input1, 
    double interpolationFactor,
    double * & output
    ) {
    // STFT
    stft0.processHop(input0);
    stft1.processHop(input1);

    fftw_complex * transform0 = stft0.getTransform();
    fftw_complex * transform1 = stft1.getTransform();

    // Process with virtual function
    processFrameTransform(
        transform0,
        transform1,
        interpolationFactor,
        transformOut
        );

    // ISTFT
    istft.processTransform(transformOut);

    // Return
    output = istft.getAudio();
}
