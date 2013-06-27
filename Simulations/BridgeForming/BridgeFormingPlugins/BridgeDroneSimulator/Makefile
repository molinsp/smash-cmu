CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
INC = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I../V-RepImports -I../../../../DroneController/packages/madara_client/bridge -I../../../../DroneController/packages/madara_client/utilities -I../../../../DroneController/packages/madara_client/area_coverage
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux

all: BridgeDroneSimulator

BridgeDroneSimulator: Main.o
	$(CC) $(STD) $(WARN) $(OPT) $(INC) $(LIB_PATH) $(OS) -o BridgeDroneSimulator Main.o ../../../../DroneController/packages/madara_client/bridge/BridgeAlgorithm.cpp ../../../../DroneController/packages/madara_client/bridge/bridge_module.cpp ../../../../DroneController/packages/madara_client/area_coverage/SimpleAreaCoverage.cpp ../../../../DroneController/packages/madara_client/area_coverage/area_coverage_module.cpp -lACE -lMADARA -ldl -lrt

Main.o: Main.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(INC) $(OS) -c -o Main.o Main.cpp

clean:
	rm -f Main.o
	rm -f BridgeDroneSimulator
