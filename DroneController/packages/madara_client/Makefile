######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################


CC = $(LOCAL_GPLUSPLUS)

LOCAL_EXECUTABLE = madara_client

LOCAL_SOURCES = madara_client.cpp drone_controller.cpp

LOCAL_CFLAGS = -D$(PLATFORM) -ggdb
LOCAL_LIBS = -lMADARA -lACE
LOCAL_LD_PATH = -L$(INSTALL_ROOT)/lib
LOCAL_INCLUDES = -I$(INSTALL_ROOT)/include -I$(CURDIR)

include area_coverage/area_coverage_module.mk
include movement/movement_module.mk
include sensors/sensors_module.mk
include bridge/bridge_module.mk
include utilities/utilities_module.mk

ifdef PLATFORM
include platforms/$(PLATFORM).mk
endif

LOCAL_OBJ = $(LOCAL_SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

ifeq ($(TARGET),AR_DRONE_2)
TEST_BROADCAST_SOURCES = platforms/ar_drone_2/test_broadcast.cpp transport/DroneRK_Transport.cpp transport/DroneRK_Transport_Read_Thread.cpp
TEST_BROADCAST_OBJ = $(TEST_BROADCAST_SOURCES:.cpp=.o)
TEST_LISTEN_BROADCAST_SOURCES = platforms/ar_drone_2/test_listen_broadcast.cpp
TEST_LISTEN_BROADCAST_OBJ = $(TEST_LISTEN_BROADCAST_SOURCES:.cpp=.o)
default: $(LOCAL_EXECUTABLE)
else
default: $(LOCAL_EXECUTABLE)
endif

$(LOCAL_EXECUTABLE): $(LOCAL_SOURCES) $(LOCAL_OBJ)
	@mkdir -p bin
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_CFLAGS) $(LOCAL_OBJ) -o bin/$@ $(LOCAL_LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)$(OK)"; \
	else echo "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi
	
	@_ERROR=$$(mkdir -p $(INSTALL_ROOT)/bin; cp bin/$(LOCAL_EXECUTABLE) $(INSTALL_ROOT)/bin/; chmod 755 $(INSTALL_ROOT)/bin/$(EXECUTABLE)); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Installing $(LOCAL_EXECUTABLE)...$(RESET)$(OK)"; \
	else echo "$(BOLD)Installing $(LOCAL_EXECUTABLE)...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

.cpp.o:
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_INCLUDES) $(LOCAL_CFLAGS) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Compiling $<...$(RESET)$(OK)"; \
	else echo "$(BOLD)Compiling $<...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

test_broadcast_custom_transport: $(TEST_BROADCAST_SOURCES) $(TEST_BROADCAST_OBJ)
	@mkdir -p bin
	@_ERROR=$$($(CC) -I$(INSTALL_ROOT)/include $(LOCAL_LD_PATH) $(TEST_BROADCAST_OBJ) -o bin/$@ -lACE -lMADARA -ldrk 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Linking $@...$(RESET)$(OK)"; \
	else echo "$(BOLD)Linking $@...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi
	
	@_ERROR=$$(mkdir -p $(INSTALL_ROOT)/bin; cp bin/$@ $(INSTALL_ROOT)/bin/; chmod 755 $(INSTALL_ROOT)/bin/$@); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Installing $@...$(RESET)$(OK)"; \
	else echo "$(BOLD)Installing $@...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

test_listen_broadcast: $(TEST_LISTEN_BROADCAST_SOURCES) $(TEST_LISTEN_BROADCAST_OBJ)
	@mkdir -p bin
	@_ERROR=$$($(CC) -I$(INSTALL_ROOT)/include $(LOCAL_LD_PATH) $(TEST_BROADCAST_OBJ) -o bin/$@ -lACE -lMADARA -ldrk 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Linking $@...$(RESET)$(OK)"; \
	else echo "$(BOLD)Linking $@...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi
	
	@_ERROR=$$(mkdir -p $(INSTALL_ROOT)/bin; cp bin/$@ $(INSTALL_ROOT)/bin/; chmod 755 $(INSTALL_ROOT)/bin/$@); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Installing $@...$(RESET)$(OK)"; \
	else echo "$(BOLD)Installing $@...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

update:
	@echo "$(BOLD)madara_client updated automatically$(RESET)"

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(TEST_BROADCAST_OBJ)
	for f in `find . -regex .*\.o`; do rm $$f; done

realclean: clean
	rm -rf bin
