// Tester.cpp : Defines the entry point for the console application.
//

#include "utilities/gps_utils.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  SMASH::Utilities::Position initPos(40.44112049,-79.9471758);
  cout << "Init pos: " << initPos.toString() << endl;

  SMASH::Utilities::Position referencePoint(40.44108, -79.947164);
  SMASH::Utilities::CartesianPosition cartPos = SMASH::Utilities::getCartesianCoordinates(initPos, referencePoint);
  cout << "Cart pos: " << cartPos.toString() << endl;

  SMASH::Utilities::Position returnPos = SMASH::Utilities::getLatAndLong(cartPos.x, cartPos.y, referencePoint);
  cout << "Retu pos: " << returnPos.toString() << endl;

  double dist = SMASH::Utilities::gps_coordinates_distance(initPos.latitude, initPos.longitude, returnPos.latitude, returnPos.longitude);
  cout << "Dist: " << dist << " meters." << endl;
}

