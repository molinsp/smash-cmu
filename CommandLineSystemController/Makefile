######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

CC = g++

LOCAL_EXECUTABLE = simple_coverage_test

LOCAL_SOURCES = simple_coverage_test.cpp

REPO_ROOT = ../
MADARA_CLIENT_ROOT = $(REPO_ROOT)/DroneController/packages/madara_client
LOCAL_CFLAGS = -ggdb
LOCAL_LIBS = -lMADARA -lACE
LOCAL_LD_PATH = -L$(MADARA_ROOT)/lib -L$(ACE_ROOT)/lib
LOCAL_INCLUDES = -I$(MADARA_ROOT)/include -I$(ACE_ROOT) -I$(MADARA_CLIENT_ROOT)

LOCAL_SOURCES += $(MADARA_CLIENT_ROOT)/comm/*.cpp
LOCAL_SOURCES += $(MADARA_CLIENT_ROOT)/comm/multicast/*.cpp

LOCAL_OBJ = $(LOCAL_SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

$(LOCAL_EXECUTABLE): $(LOCAL_SOURCES) $(LOCAL_OBJ)
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_CFLAGS) $(LOCAL_OBJ) -o $@ $(LOCAL_LIBS) 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)$(OK)"; \
	else echo "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi
	
.cpp.o:
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_INCLUDES) $(LOCAL_CFLAGS) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(BOLD)Compiling $<...$(RESET)$(OK)"; \
	else echo "$(BOLD)Compiling $<...$(RESET)$(FAIL)\n$$_ERROR"; exit 1; fi

.PHONY: clean
clean:
	rm -f *.o

realclean: clean
	rm -rf $(LOCAL_EXECUTABLE)
