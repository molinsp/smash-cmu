CC = g++
STD = --std=c++0x
WARN = -Wall
OPT = -O3
#DEBUG = -ggdb
ifndef REPO_ROOT
  REPO_ROOT=$(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I../V-RepCommonPluginUtils -I../WindowsMulticastTransport -I../V-RepImports -I$(MADARA_ROOT)/include -I$(MADARA_CLIENT) -I$(ACE_ROOT)
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
LIBS = -lMADARA -lACE
OS = -D__linux

SOURCES = ../V-RepCommonPluginUtils/LuaExtensionsUtils.cpp ../V-RepCommonPluginUtils/v_repExtLuaExtenderPlugin.cpp ../V-RepImports/v_repLib.cpp LuaCustomFunctions.cpp MadaraQuadrotorControl.cpp

OBJ = $(SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

all: libv_repExtMadaraQuadrotorControlPlugin.so

libv_repExtMadaraQuadrotorControlPlugin.so: $(SOURCES) $(OBJ)
	@echo -n "$(BOLD)Linking $@...$(RESET)"
	@_ERROR=$$($(CC) $(STD) -fPIC -shared $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) $(LIB_PATH) $(OBJ) -o $@ $(LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi
	
.cpp.o:
	@echo -n "$(BOLD)Compiling $<...$(RESET)"
	@_ERROR=$$($(CC) $(STD) -fPIC $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

clean:
	rm -f LuaCustomFunctions.o
	rm -f MadaraQuadrotorControl.o
	rm -f v_repExtMadaraQuadrotorControlPlugin.o
	rm -f v_repLib.o

realclean: clean
	rm -f libv_repExtMadaraQuadrotorControlPlugin.so
