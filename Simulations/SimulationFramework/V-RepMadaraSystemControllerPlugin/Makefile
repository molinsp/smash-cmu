CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
ifndef REPO_ROOT
  REPO_ROOT = $(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I../V-RepImports -I$(ACE_ROOT) -I$(MADARA_ROOT)/include -I$(MADARA_CLIENT)
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux

all: libv_repExtMadaraSystemControllerPlugin.so

libv_repExtMadaraSystemControllerPlugin.so: v_repExtMadaraSystemController.o ../V-RepImports/v_repLib.cpp MadaraSystemController.o
	$(CC) $(STD) $(WARN) $(OS) $(INC) $(LIB_PATH) -fPIC -shared -o libv_repExtMadaraSystemControllerPlugin.so v_repExtMadaraSystemController.o ../V-RepImports/v_repLib.cpp LuaCustomFunctions.o MadaraSystemController.o -lMADARA -lACE -ldl -lrt

v_repExtMadaraSystemController.o: v_repExtMadaraSystemController.cpp v_repExtMadaraSystemController.h LuaCustomFunctions.o
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o v_repExtMadaraSystemController.o v_repExtMadaraSystemController.cpp
	
MadaraSystemController.o: MadaraSystemController.cpp
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o MadaraSystemController.o MadaraSystemController.cpp

LuaCustomFunctions.o: LuaCustomFunctions.cpp LuaCustomFunctions.h
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o LuaCustomFunctions.o LuaCustomFunctions.cpp

clean:
	rm *.o *.so
