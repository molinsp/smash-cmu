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

libv_repExtMadaraSystemControllerPlugin.so: v_repExtMadaraSystemController.o ../V-RepImports/v_repLib.cpp ../V-RepMadaraClientPlugin/MadaraController.cpp
	$(CC) $(STD) $(WARN) $(OS) $(INC) $(LIB_PATH) -fPIC -shared -o libv_repExtMadaraSystemControllerPlugin.so v_repExtMadaraSystemController.o ../V-RepImports/v_repLib.cpp LuaCustomFunctions.o ../V-RepMadaraClientPlugin/MadaraController.cpp -lMADARA -lACE -ldl -lrt

v_repExtMadaraSystemController.o: v_repExtBridgeMadaraClient.cpp v_repExtBridgeMadaraClient.h LuaCustomFunctions.o
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o v_repExtMadaraSystemController.o v_repExtBridgeMadaraClient.cpp
	
MadaraController.o: MadaraController.cpp
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o MadaraController.o MadaraController.cpp

LuaCustomFunctions.o: LuaCustomFunctions.cpp LuaCustomFunctions.h
	$(CC) $(STD) $(WARN) $(OS) $(INC) -fPIC -c -o LuaCustomFunctions.o LuaCustomFunctions.cpp

clean:
	rm -f LuaCustomFunctions.o
	rm -f MadaraController.o
	rm -f v_repExtMadaraSystemController.o
	rm -f libv_repExtMadaraSystemControllerPlugin.so
