CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
#OPT = -O3
DEBUG = -ggdb
ifndef REPO_ROOT
  REPO_ROOT=$(HOME)/dev/smash-cmu
endif
INC = -I../V-RepImports -I$(ACE_ROOT) -I$(MADARA_ROOT)/include -I$(REPO_ROOT)/DroneController/packages/madara_client
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux

all: libv_repExtMadaraQuadrotorControlPlugin.so

libv_repExtMadaraQuadrotorControlPlugin.so: v_repExtMadaraQuadrotorControlPlugin.o ../V-RepImports/v_repLib.cpp MadaraQuadrotorControl.o LuaCustomFunctions.o v_repLib.o
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(OS) $(INC) $(LIB_PATH) -fPIC -shared -o libv_repExtMadaraQuadrotorControlPlugin.so v_repExtMadaraQuadrotorControlPlugin.o v_repLib.o LuaCustomFunctions.o MadaraQuadrotorControl.o -lMADARA -lACE -ldl -lrt

v_repLib.o: ../V-RepImports/v_repLib.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(OS) $(INC) -fPIC -c -o v_repLib.o ../V-RepImports/v_repLib.cpp > /dev/null 2> /dev/null

v_repExtMadaraQuadrotorControlPlugin.o: v_repExtMadaraQuadrotorControlPlugin.cpp v_repExtMadaraQuadrotorControlPlugin.h LuaCustomFunctions.o
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(OS) $(INC) -fPIC -c -o v_repExtMadaraQuadrotorControlPlugin.o v_repExtMadaraQuadrotorControlPlugin.cpp
	
MadaraQuadrotorControl.o: MadaraQuadrotorControl.cpp MadaraQuadrotorControl.h
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(OS) $(INC) -fPIC -c -o MadaraQuadrotorControl.o MadaraQuadrotorControl.cpp

LuaCustomFunctions.o: LuaCustomFunctions.cpp LuaCustomFunctions.h MadaraQuadrotorControl.h 
	$(CC) $(STD) $(WARN) $(OPT) $(DEBUG) $(OS) $(INC) -fPIC -c -o LuaCustomFunctions.o LuaCustomFunctions.cpp

clean:
	rm -f LuaCustomFunctions.o
	rm -f MadaraQuadrotorControl.o
	rm -f v_repExtMadaraQuadrotorControlPlugin.o
	rm -f v_repLib.o
	rm -f libv_repExtMadaraQuadrotorControlPlugin.so
