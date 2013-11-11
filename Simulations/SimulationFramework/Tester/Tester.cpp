// Tester.cpp : Defines the entry point for the console application.
//

#include "utilities/gps_utils.h"
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{
  SMASH::Utilities::Position initPos(40.44115998,-79.94717768);
  cout << "Init pos: " << initPos.toString() << endl;

  SMASH::Utilities::Position referencePoint(40.44108, -79.947164);
  SMASH::Utilities::CartesianPosition cartPos = SMASH::Utilities::getCartesianCoordinates(initPos, referencePoint);
  cout << "Cart pos: " << cartPos.toString() << endl;

  SMASH::Utilities::Position endPos(40.44112049,-79.9471758);
  cout << "End pos: " << initPos.toString() << endl;

  SMASH::Utilities::CartesianPosition endCartPos = SMASH::Utilities::getCartesianCoordinates(endPos, referencePoint);
  cout << "Cart pos: " << endCartPos.toString() << endl;
  
  double dist = SMASH::Utilities::gps_coordinates_distance(initPos.latitude, initPos.longitude, endPos.latitude, endPos.longitude);
  cout << "Dist: " << dist << " meters." << endl;

  double distCart = sqrt(pow((cartPos.x - endCartPos.x),2)) + sqrt(pow((cartPos.y-endCartPos.y),2));
  cout << "Dist: " << distCart << " meters." << endl;
}

