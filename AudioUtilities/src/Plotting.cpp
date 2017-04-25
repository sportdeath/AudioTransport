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

  plotLine(xyPoints);
}

std::vector<std::pair<double, double> > Plotting::vectorToPair(
    std::vector<double> vector,
    double xAxisScale, 
    double xAxisOffset) {

  std::vector<std::pair<double, double> > pair(vector.size());

  for (int i = 0; i < vector.size(); i++) {
    pair[i] = std::make_pair(i * xAxisScale + xAxisOffset, vector[i]);
  }

  return pair;
}

std::vector<std::vector<std::pair<double, double> > > Plotting::vectorsToPairs(
    std::vector<std::vector<double> > vectors,
    double xAxisScale, 
    double xAxisOffset) {
  std::vector<std::vector<std::pair<double, double> > > pairs(vectors.size());

  for (int i = 0; i < vectors[i].size(); i++) {
    pairs[i] = vectorToPair(vectors[i], xAxisScale, xAxisOffset);
  }

  return pairs;
}

void Plotting::plotVectors(
    std::vector<std::vector<double> > lines,
    double xAxisScale, 
    double xAxisOffset) {
  Gnuplot gp;
  gp << "plot";
  for (int i = 0; i < lines.size(); i++) {
    gp << gp.file1d(vectorToPair(lines[i])) << "w l";
    if (i != lines.size() - 1) {
      gp << ",";
    }
  } 
  gp << std::endl;
  std::cin.get();
}

void Plotting::plotVector(
    std::vector<double> vector, 
    double xAxisScale, 
    double xAxisOffset) {

  plotLine(vectorToPair(vector, xAxisScale, xAxisOffset));
}

void Plotting::plotLine(std::vector<std::pair<double, double> > line) {
  Gnuplot gp;
  gp << "plot" << gp.file1d(line) << "w l" << std::endl;
  std::cin.get();
}

void Plotting::plotLines(std::vector<std::vector<std::pair<double, double> > > lines) {
  Gnuplot gp;
  gp << "plot";
  for (int i = 0; i < lines.size(); i++) {
    gp << gp.file1d(lines[i]) << "w l";
    if (i != lines.size() - 1) {
      gp << ",";
    }
  } 
  gp << std::endl;
  std::cin.get();
}

void Plotting::plotLinesAndMarkers(
    std::vector<std::vector<double> > lines,
    std::vector<std::vector<std::pair<double, double> > > markers
    ) {
  Gnuplot gp;
  gp << "plot";
  for (int i = 0; i < lines.size(); i++) {
    gp << gp.file1d(vectorToPair(lines[i])) << "w l";
    gp << ",";
  } 
  for (int i = 0; i < markers.size(); i++) {
    gp << gp.file1d(markers[i]) << "w p";
    if (i != markers.size() - 1) {
      gp << ",";
    }
  } 
  gp << std::endl;
  std::cin.get();
}
