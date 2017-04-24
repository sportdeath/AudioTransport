#include <vector>

#include "AudioUtilities/Plotting.hpp"
#include "AudioUtilities/gnuplot-iostream.h"

void Plotting::plotArray(
    double * array, 
    unsigned int size, 
    double xAxisScale, 
    double xAxisOffset) {
  std::vector<std::pair<double, double> > xyPoints(size);
  for (long i = 0; i < size; i++) {
    xyPoints[i] = std::make_pair(i * xAxisScale + xAxisOffset, array[i]);
  }

  plotPair(xyPoints);
}

void Plotting::plotVectors(
    std::vector<std::vector<double> > vectors,
    double xAxisScale, 
    double xAxisOffset) {
  std::vector<std::vector<std::pair<double, double> > > pairs(vectors.size());
  for (int i = 0; i < vectors.size(); i++) {
    pairs[i].resize(vectors[i].size());
    for (int j = 0; j < vectors[i].size(); j++) {
      pairs[i][j] = std::make_pair(j * xAxisScale + xAxisOffset, vectors[i][j]);
    }
  }

  plotPairs(pairs);
}

void Plotting::plotVector(std::vector<double> y, 
                     double xAxisScale, 
                     double xAxisOffset) {

  std::vector<std::pair<double, double> > xyPoints(y.size());
  for (long i = 0; i < y.size(); i++) {
    xyPoints[i] = std::make_pair(i * xAxisScale + xAxisOffset, y[i]);
  }

  plotPair(xyPoints);
}

void Plotting::plotPair(std::vector<std::pair<double, double> > xy) {
  Gnuplot gp;
  gp << "plot" << gp.file1d(xy) << "w l" << std::endl;
  std::cin.get();
}

void Plotting::plotPairs(std::vector<std::vector<std::pair<double, double> > > pairs) {
  Gnuplot gp;
  gp << "plot";
  for (int i = 0; i < pairs.size(); i++) {
    gp << gp.file1d(pairs[i]) << "w l";
    if (i != pairs.size() - 1) {
      gp << ",";
    }
  } 
  gp << std::endl;
  std::cin.get();
}

void Plotting::plotLineAndMarkers(
    std::vector<std::pair<double, double> > line,
    std::vector<double> markers,
    double pointHeight) {

  std::vector<std::pair<double, double> > markerPoints(markers.size());
  for (long i = 0; i < markerPoints.size(); i++) {
    markerPoints[i] = std::make_pair(markers[i], pointHeight);
  }

  Gnuplot gp;
  gp << "plot '-' w l, '-' w p" << std::endl;
  gp.send1d(line);
  gp.send1d(markerPoints);
  std::cin.get();
}

