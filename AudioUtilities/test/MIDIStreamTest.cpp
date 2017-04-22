#include <iostream>
#include <chrono>
#include <thread>

#include <AudioUtilities/MIDIStream.hpp>
#include <AudioUtilities/ChooseMIDIDevices.hpp>

int main() {
  // open stream

  int device = chooseMIDIDevices();

  std::vector<int> controllerValues(4);
  controllerValues[0] = 64;
  controllerValues[1] = 48;
  controllerValues[2] = 49;
  controllerValues[3] = 50;

  MIDIStream stream(device,controllerValues);

  int millisecondsToSleep = 100;
  while (true)  {
    stream.updateValues();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(millisecondsToSleep)
        );
    for (int i = 0; i < 4; i++) {
      std::cout << i << ": " << stream.getValue(controllerValues[i]) << std::endl;
    }
  }
}
