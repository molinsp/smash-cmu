CC = g++
STD = --std=c++0x
WARN = -Wall -Wextra -pedantic
OPT = -O3
DEBUG = 
INC = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I../V-RepImports -I../../../../DroneController/packages/madara_client
LIB_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
OS = -D__linux

all: libv_repExtBridgeAlgorithm.so

libv_repExtBridgeAlgorithm.so: v_repExtBridgeAlgorithm.o ../V-RepImports/v_repLib.cpp ../V-RepMadaraClientPlugin/MadaraController.cpp
	$(CC) $(STD) $(WARN) $(OPT) $(OS) $(INC) $(LIB_PATH) -fPIC -shared -o libv_repExtBridgeAlgorithm.so v_repExtBridgeAlgorithm.o ../V-RepImports/v_repLib.cpp ../V-RepMadaraClientPlugin/MadaraController.cpp -lMADARA -lACE -ldl -lrt

v_repExtBridgeAlgorithm.o: v_repExtBridgeAlgorithm.cpp v_repExtBridgeAlgorithm.h
	$(CC) $(STD) $(WARN) $(OPT) $(OS) $(INC) -fPIC -c -o v_repExtBridgeAlgorithm.o v_repExtBridgeAlgorithm.cpp

clean:
	rm -f v_repExtBridgeAlgorithm.o
	rm -f libv_repExtBridgeAlgorithm.so
