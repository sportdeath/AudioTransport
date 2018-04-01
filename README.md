# Audio Transport

An implimentation of included [paper](https://github.com/sportdeath/AudioTransport/raw/master/paper/paper.pdf), which 
describes a new technique to interpolate between audio
signals. This technique uses optimal transport to move the pitches in
one signal to the pitches in another. Audibly this sounds like a generalized
portamento, or glide between sounds. The main contributions
of this paper are methods to remove the different types of phasing
distortion that can occur in a naive transport. This implimentation works
in real time and produces musical results. 

Click on the video below for a demonstration.

[![video](http://img.youtube.com/vi/PQGV0fk3Gww/0.jpg)](https://www.youtube.com/watch?v=PQGV0fk3Gww)

## Usage
    
Build the binaries using cmake:

    $ cd AudioTransport
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    
Run the selected binary and then navigate throught the inputs.
If you want to use a DAW rather than an external MIDI controller to control the transport,
you will need to set up a [virtual MIDI bus](https://help.ableton.com/hc/en-us/articles/209774225-Using-virtual-MIDI-buses).
On OS X, you can use [Soundflower](https://github.com/mattingalls/Soundflower) for internal audio routing.

    $ ./BidirectionalTransportVocoderTest 
    
    0: Soundflower (64ch)
    
    Select an input device: 0
    Using Device: Soundflower (64ch)
    
    0: Built-in Output
    1: Soundflower (2ch)
    2: Soundflower (64ch)
    
    Select an output device: 0
    Using Device: Built-in Output
    
    0: IAC Driver Bus 1
    
    Select an input device: 0
    Using Device: IAC Driver Bus 1

    To quit type 'quit': quit
    
Play audio on channels 3/4 and 5/6 of the input audio device. 
Use midi value 64 on channel 1 of the input MIDI device to interpolate between the two audio signals.
These defaults can be changed in the corresponding [script file](https://github.com/sportdeath/AudioTransport/blob/master/test/BidirectionalTransportVocoderTest.cpp).

## Dependencies

- cmake
- fftw3
- portaudio
- portmidi
