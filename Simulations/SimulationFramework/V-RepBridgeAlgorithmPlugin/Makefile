CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
ifndef REPO_ROOT
  REPO_ROOT=$(HOME)/dev/smash-cmu
endif
MADARA_CLIENT = $(REPO_ROOT)/DroneController/packages/madara_client
INC = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I../V-RepImports -I$(MADARA_CLIENT)
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux

all:
	@echo "nothing to do here"

libv_repExtBridgeAlgorithm.so: v_repExtBridgeAlgorithm.o libv_rep.o ../V-RepMadaraQuadrotorControlPlugin/MadaraQuadrotorControl.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(OS) $(INC) $(LIB_PATH) -fPIC -shared -o libv_repExtBridgeAlgorithm.so v_repExtBridgeAlgorithm.o libv_rep.o ../V-RepMadaraQuadrotorControlPlugin/MadaraQuadrotorControl.cpp -lMADARA -lACE -ldl -lrt

v_repExtBridgeAlgorithm.o: v_repExtBridgeAlgorithm.cpp v_repExtBridgeAlgorithm.h
	$(CC) $(STD) $(WARN) $(OPT) $(OS) $(INC) -fPIC -c -o v_repExtBridgeAlgorithm.o v_repExtBridgeAlgorithm.cpp

libv_rep.o: ../V-RepImports/v_repLib.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(OS) $(INC) -fPIC -c -o libv_rep.o ../V-RepImports/v_repLib.cpp > /dev/null 2> /dev/null

clean:
	rm -f libv_rep.o
	rm -f v_repExtBridgeAlgorithm.o

realclean: clean
	rm -f libv_repExtBridgeAlgorithm.so
