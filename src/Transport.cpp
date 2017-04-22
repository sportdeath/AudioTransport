struct SpectralPiece {
  int startIndex;
  int length;
  double mass;
  double centerOfMass; // relative to start bin
}

class Transport {
  public:
  private:
    Transport(unsigned int TransportSize)

    updateMass();
    massAssignment(
        spectralPieces1,
}

void massAssignment(
    SpectralPiece * spectralPieces, 
    int numPieces) {
  numAssignments = 0;

  int index0 = 0;
  int index1 = 0;
  double mass0 = masses0[index0];
  double mass1 = masses1[index1];

  while (true) {
    // save the indexes
    assignmentIndices0[numAssignments] = index0;
    assignmentIndices1[numAssignments] = index1;

    // If mass 0 is small we use it up completely
    // otherwise we use mass 1 up completely
    if (mass0 < mass1) {
      assignmentMasses[numAssignments] = mass0;
    } else {
      assignmentMasses[numAssignments] = mass1;
    }

    // Subtract from mass
    // Guaranteed to be positive or zero
    mass0 -= assignmentMasses[numAssignments];
    mass1 -= assignmentMasses[numAssignments];

    // Update the number of assignments
    numAssignments++;

    // If either mass is now zero
    // update the index and the mass
    // If the index reaches the end, we break
    if (not updateMass(index0, mass0, masses0)) break;
    if (not updateMass(index1, mass1, masses1)) break;
  }
}

// Seperate complex transform into masses and angles

// Compute the group delay derivative

// Use group delay derivative to segment mass into masschunks

// compute optimal transport on the chunks

// For assignment
// compute new bin given centers of mass and stuff
// (start bin typa deal)
// multiply by assigmnet/mass

// for i in max size centered around both
// add 1 to masses 
// if one doesn't exist, use mass 1, 
// and phase = phase of first thing (should be accumulated)
// subtract 1 after multiplication.
// add to the correct bin

// output!

};

