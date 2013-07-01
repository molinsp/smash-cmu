CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
#OPT = -O3
DEBUG = -ggdb
ifndef REPO_ROOT
  REPO_ROOT=$(HOME)/dev/smash-cmu
endif
INC = -I$(ACE_ROOT) -I$(MADARA_ROOT)/include -I$(REPO_ROOT)/DroneController/packages/madara_client
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib -L.

all: libMadaraQuadrotorControl.so GoToLocation

libMadaraQuadrotorControl.so: MadaraQuadrotorControl.o
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) $(LIB_PATH) -fPIC -shared -o libMadaraQuadrotorControl.so MadaraQuadrotorControl.o -lMADARA -lACE -ldl -lrt

MadaraQuadrotorControl.o: MadaraQuadrotorControl.cpp MadaraQuadrotorControl.h
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) -fPIC -c -o MadaraQuadrotorControl.o MadaraQuadrotorControl.cpp

GoToLocation: GoToLocation.o libMadaraQuadrotorControl.so
	$(CC) $(STD) $(OPT) $(DEBUG) $(WARN) -pthread -pipe -Wl,-E $(LIB_PATH) -o GoToLocation GoToLocation.o -lMadaraQuadrotorControl -lACE -lMADARA -ldl -lrt

GoToLocation.o: GoToLocation.cpp ../MadaraQuadrotorControl/MadaraQuadrotorControl.h
	$(CC) $(STD) $(OPT) $(DEBUG) $(WARN) -pthread -pipe $(INC) -c -o GoToLocation.o GoToLocation.cpp

clean:
	rm -f GoToLocation.o
	rm -f GoToLocation
	rm -f MadaraQuadrotorControl.o
	rm -f libMadaraQuadrotorControl.so
