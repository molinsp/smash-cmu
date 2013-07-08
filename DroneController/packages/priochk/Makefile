######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################


CC = $(LOCAL_GPLUSPLUS)

LOCAL_EXECUTABLE = priochk
LOCAL_SOURCES = priochk.cpp
LOCAL_CFLAGS =
LOCAL_LD_PATH = -L$(INSTALL_ROOT)/lib
LOCAL_INCLUDES = -I$(INSTALL_ROOT)/include -I$(CURDIR)

LOCAL_OBJ = $(LOCAL_SOURCES:.cpp=.o)

BOLD  = $(shell tput bold)
RED   = $(shell tput setaf 1)
GREEN = $(shell tput setaf 2)
RESET = $(shell tput sgr0)

OK    = $(BOLD)$(GREEN)[OK]$(RESET)
FAIL  = $(BOLD)$(RED)[FAIL]$(RESET)

default: $(LOCAL_EXECUTABLE)

$(LOCAL_EXECUTABLE): $(LOCAL_SOURCES) $(LOCAL_OBJ)
	@echo -n "$(BOLD)Linking $(LOCAL_EXECUTABLE)...$(RESET)"
	@mkdir -p bin
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_CFLAGS) $(LOCAL_OBJ) -o bin/$@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi
	@mkdir -p $(INSTALL_ROOT)/bin
	@cp bin/$(LOCAL_EXECUTABLE) $(INSTALL_ROOT)/bin/
	@chmod 755 $(INSTALL_ROOT)/bin/$(EXECUTABLE)

.cpp.o:
	@echo -n "$(BOLD)Compiling $<...$(RESET)"
	@_ERROR=$$($(CC) $(LOCAL_LD_PATH) $(LOCAL_INCLUDES) $(LOCAL_CFLAGS) -c $< -o $@ 2>&1); \
	if [ $$? -eq 0 ]; then echo "$(OK)"; \
	else echo "$(FAIL)\n$$_ERROR"; exit 1; fi

.PHONY: clean
clean:
	rm -rf bin
	rm -rf $(LOCAL_OBJ)

realclean: clean
	rm -f bin/$(LOCAL_EXECUTABLE)
