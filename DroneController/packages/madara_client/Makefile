######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################


CC = $(LOCAL_GPLUSPLUS)

LOCAL_EXECUTABLE = madara_client

LOCAL_SOURCES = madara_client.cpp main_functions.cpp

LOCAL_CFLAGS = -D$(PLATFORM) -ggdb
LOCAL_LIBS = -lMADARA -lACE
LOCAL_LD_PATH = -L$(INSTALL_ROOT)/lib
LOCAL_INCLUDES = -I$(INSTALL_ROOT)/include -I$(CURDIR)

include area_coverage/area_coverage_module.mk
include movement/movement_module.mk
include sensors/sensors_module.mk
include bridge/bridge_module.mk
include utilities/utilities_module.mk

#will need to modify based on desired transport settings
include transport/transport_module.mk

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
default: test_broadcast_custom_transport $(LOCAL_EXECUTABLE)
else
default: $(LOCAL_EXECUTABLE)
endif

$(LOCAL_EXECUTABLE): $(LOCAL_SOURCES) $(LOCAL_OBJ)
	@echo -n "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)"
	@mkdir -p bin
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_CFLAGS) $(LOCAL_OBJ) -o bin/$@ $(LOCAL_LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi
	
	@echo -n "$(BOLD)Installing $(LOCAL_EXECUTABLE)...$(RESET)"
	@_ERROR=$$(mkdir -p $(INSTALL_ROOT)/bin; cp bin/$(LOCAL_EXECUTABLE) $(INSTALL_ROOT)/bin/; chmod 755 $(INSTALL_ROOT)/bin/$(EXECUTABLE)); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

.cpp.o:
	@echo -n "$(BOLD)Compiling $<...$(RESET)"
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_INCLUDES) $(LOCAL_CFLAGS) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

test_broadcast_custom_transport: $(TEST_BROADCAST_SOURCES) $(TEST_BROADCAST_OBJ)
	@echo -n "$(BOLD)Linking $@...$(RESET)"
	@mkdir -p bin
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(TEST_BROADCAST_OBJ) -o bin/$@ -lACE -lMADARA 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

.PHONY: clean
clean:
	rm -rf bin
	rm -rf $(LOCAL_OBJ)

realclean: clean
	rm -f bin/$(LOCAL_EXECUTABLE)
	rm -f test_broadcast_custom_transport
