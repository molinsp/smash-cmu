CC = g++
STD = --std=c++0x
WARN = -Wall #-Wextra -pedantic
OPT = -O3
DEBUG = 
ifndef REPO_ROOT
  REPO_ROOT = $(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I../V-RepImports -I$(MADARA_CLIENT)
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
LIBS = -lMADARA -lACE
OS = -D__linux
PLATFORM = -DV_REP
MADARA_FILES = $(MADARA_CLIENT)/bridge/BridgeAlgorithm.cpp $(MADARA_CLIENT)/bridge/bridge_module.cpp $(MADARA_CLIENT)/area_coverage/SnakeAreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/RandomAreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/InsideOutAreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/AreaCoverage.cpp $(MADARA_CLIENT)/area_coverage/area_coverage_module.cpp $(MADARA_CLIENT)/sensors/sensors_module.cpp $(MADARA_CLIENT)/platforms/v_rep/v-rep_control_functions.cpp $(MADARA_CLIENT)/movement/movement_module.cpp $(MADARA_CLIENT)/utilities/utilities_module.cpp $(MADARA_CLIENT)/main_functions.cpp

SOURCES = $(MADARA_FILES) Main.cpp

OBJ = $(SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

all: DroneControllerSimulator

DroneControllerSimulator: $(SOURCES) $(OBJ)
	@_ERROR=$$($(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) $(PLATFORM) $(LIB_PATH) $(OBJ) -o $@ $(LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Linking $@...$(RESET)$(OK)"; \
	else echo "$(BOLD)Linking $@...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

.cpp.o:
	@_ERROR=$$($(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) $(PLATFORM) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Compiling $<...$(RESET)$(OK)"; \
	else echo "$(BOLD)Compiling $<...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

clean:
	rm -f Main.o
	make -C $(REPO_ROOT)/DroneController/packages/madara_client clean

realclean: clean
	rm -f DroneControllerSimulator
	make -C $(REPO_ROOT)/DroneController/packages/madara_client realclean
