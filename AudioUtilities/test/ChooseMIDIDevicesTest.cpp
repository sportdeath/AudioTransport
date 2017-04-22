#include <iostream>

#include <AudioUtilities/ChooseMIDIDevices.hpp>

int main() {

  int device = chooseMIDIDevices();

  std::cout << "Chose device: " << device << std::endl;
}
