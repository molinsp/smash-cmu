CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = 
DEBUG = -ggdb
INC = -I$(ACE_ROOT) -I$(MADARA_ROOT)/include -I$(HOME)/dev/smash-cmu/DroneController/packages/madara_client
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib

all: GoToLocation

GoToLocation: GoToLocation.o
	$(CC) $(STD) $(OPT) $(DEBUG) $(WARN) -pthread -pipe -Wl,-E $(LIB_PATH) -o GoToLocation GoToLocation.o -lACE -lMADARA -ldl -lrt

GoToLocation.o: GoToLocation.cpp
	$(CC) $(STD) $(OPT) $(DEBUG) $(WARN) -pthread -pipe $(INC) -c -o GoToLocation.o GoToLocation.cpp

clean:
	rm -f GoToLocation.o
	rm -f GoToLocation
