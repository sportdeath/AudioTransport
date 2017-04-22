#define BUFFER_SIZE 100

#include <vector>
#include <map>

#include <portmidi.h>

class MIDIStream {
  private:
    std::map<int, double> values;
    PortMidiStream * stream;
    PmEvent buffer[BUFFER_SIZE];

  public:
    MIDIStream(
        int deviceNum, 
        std::vector<int> controllerNumbers
        );

    ~MIDIStream();

    void updateValues();

    double getValue(int number);
};
