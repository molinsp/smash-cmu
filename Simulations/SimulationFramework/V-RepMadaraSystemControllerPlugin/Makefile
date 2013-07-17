CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
ifndef REPO_ROOT
  REPO_ROOT = $(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I../V-RepImports -I$(ACE_ROOT) -I$(MADARA_ROOT)/include -I$(MADARA_CLIENT) -I../V-RepCommonPluginUtils
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
LIBS = -lMADARA -lACE
OS = -D__linux

SOURCES = v_repExtMadaraSystemController.cpp ../V-RepImports/v_repLib.cpp MadaraSystemController.cpp LuaCustomFunctions.cpp
SOURCES = ../V-RepCommonPluginUtils/LuaExtensionsUtils.cpp ../V-RepCommonPluginUtils/v_repExtLuaExtenderPlugin.cpp ../V-RepImports/v_repLib.cpp LuaCustomFunctions.cpp MadaraSystemController.cpp
 
OBJ = $(SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

all: libv_repExtMadaraSystemControllerPlugin.so

libv_repExtMadaraSystemControllerPlugin.so: $(SOURCES) $(OBJ)
	@echo -n "$(BOLD)Linking $@...$(RESET)"
	@_ERROR=$$($(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) $(LIB_PATH) $(OBJ) -fPIC -shared -o $@ $(LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

.cpp.o:
	@echo -n "$(BOLD)Compiling $<...$(RESET)"
	@_ERROR=$$($(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(INC) $(OS) -fPIC -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

clean:
	rm -f *.o
	make -C ../V-RepImports clean
	make -C ../V-RepCommonPluginUtils clean

realclean: clean
	rm -f *.so
