/**
 * GoToLocation.cpp
 * Anton Dukeman
 *
 * Requests drone <id> move to location <x,y>
 */

#include "../MadaraQuadrotorControl/MadaraQuadrotorControl.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <string>
using std::string;

int main(int argc, char** argv)
{
  if(argc != 5)
  {
    cerr << "usage: " << argv[0] << " <id> <x> <y> <z>" << endl;
    exit(-1);
  }

  int id, x, y, z;
  sscanf(argv[1], "%d", &id);
  sscanf(argv[2], "%d", &x);
  sscanf(argv[3], "%d", &y);
  sscanf(argv[4], "%d", &z);
  cout << "request " << id << " move to " << x << "." << y << "." << z << endl;

  MadaraQuadrotorControl control(id);
  control.goToLocation(x, y, z);

  return 0;
}
