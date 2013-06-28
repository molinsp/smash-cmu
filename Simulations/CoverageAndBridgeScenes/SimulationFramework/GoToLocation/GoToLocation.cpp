/**
 * GoToLocation.cpp
 * Anton Dukeman
 *
 * Requests drone <id> move to location <x,y>
 */

#include "madara/knowledge_engine/Knowledge_Base.h"

#include "CommonMadaraVariables.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <string>
using std::string;

int main(int argc, char** argv)
{
  if(argc != 4)
  {
    cerr << "usage: " << argv[0] << " <id> <x> <y>" << endl;
    exit(-1);
  }
  Madara::Transport::Settings settings;
  settings.type = Madara::Transport::MULTICAST;
  settings.hosts_.resize(1);
  settings.hosts_[0] = "239.255.0.1:4150";
  Madara::Knowledge_Engine::Knowledge_Base knowledge("", settings);

  int id, x, y;
  sscanf(argv[1], "%d", &id);
  sscanf(argv[2], "%d", &x);
  sscanf(argv[3], "%d", &y);
  cout << "request " << id << " move to " << x << "." << y << endl;

  knowledge.set(".id", Madara::Knowledge_Record::Integer(id));
  knowledge.set(MV_MOVEMENT_REQUESTED, MO_MOVE_TO_GPS_CMD);
  knowledge.set(MV_MOVEMENT_TARGET_LAT, Madara::Knowledge_Record::Integer(x));
  knowledge.set(MV_MOVEMENT_TARGET_LON, Madara::Knowledge_Record::Integer(y));

  string eval = string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_REQUESTED) + "=" + string(MV_MOVEMENT_REQUESTED) + ";";
  eval += string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_TARGET_LAT) + "=" + string(MV_MOVEMENT_TARGET_LAT) + ";";
  eval += string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_TARGET_LON) + "=" + string(MV_MOVEMENT_TARGET_LON) + ";";

  knowledge.evaluate(eval);
  return 0;
}
