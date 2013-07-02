CC = g++
STD = --std=c++0x
#WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
ifndef REPO_ROOT
  REPO_ROOT = $(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I../V-RepImports -I$(MADARA_CLIENT)
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux
PLATFORM = -DV_REP
MADARA_FILES = $(MADARA_CLIENT)/bridge/BridgeAlgorithm.cpp $(MADARA_CLIENT)/bridge/bridge_module.cpp $(MADARA_CLIENT)/area_coverage/SnakeAreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/RandomAreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/AreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/area_coverage_module.cpp $(MADARA_CLIENT)/sensors/sensors_module.cpp $(MADARA_CLIENT)/platforms/v_rep/v-rep_control_functions.cpp $(MADARA_CLIENT)/movement/movement_module.cpp $(MADARA_CLIENT)/utilities/utilities_module.cpp

all: DroneControllerSimulator

DroneControllerSimulator: Main.o $(MADARA_FILES)
	$(CC) $(STD) $(WARN) $(OPT) $(INC) $(LIB_PATH) $(OS) $(PLATFORM) -o DroneControllerSimulator Main.o $(MADARA_FILES) -lACE -lMADARA -ldl -lrt

Main.o: Main.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(INC) $(OS) -c -o Main.o Main.cpp

clean:
	rm -f Main.o
	rm -f DroneControllerSimulator
