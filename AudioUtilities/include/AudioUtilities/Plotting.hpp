#ifndef PLOTTING
#define PLOTTING

#include <vector>

class Plotting {
  public:
    static void plotArray(double * array, unsigned int size, double xAxisScale = 1, double xAxisOffset = 0);
    static void plotVector(std::vector<double> y, double xAxisScale = 1, double xAxisOffset = 0);
    static void plotVectors(std::vector<std::vector<double> > vectors, double xAxisScale = 1, double xAxisOffset = 0);
    static void plotLine(std::vector<std::pair<double, double> > line);
    static void plotLines(std::vector<std::vector<std::pair<double, double> > > lines);
    static std::vector<std::pair<double, double> > vectorToPair(
        std::vector<double> vector,
        double xAxisScale = 1, 
        double xAxisOffset = 0);
    static std::vector<std::vector<std::pair<double, double> > > vectorsToPairs(
        std::vector<std::vector<double> > vectors,
        double xAxisScale = 1, 
        double xAxisOffset = 0);
    static void plotLinesAndMarkers(
        std::vector<std::vector<double> > lines,
        std::vector<std::vector<std::pair<double, double> > > markers
        );
};

#endif
