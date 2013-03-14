######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################


CC = $(LOCAL_GPLUSPLUS)

LOCAL_EXECUTABLE = madara_client
LOCAL_SOURCES = client.cpp \
		madara_control_functions.cpp
LOCAL_CFLAGS = -lMADARA -lACE
LOCAL_LD_PATH = -L$(INSTALL_ROOT)/lib
LOCAL_INCLUDES = -I$(INSTALL_ROOT)/include -I$(CURDIR)

include platforms/$(PLATFORM).mk

LOCAL_OBJ = $(LOCAL_SOURCES:.cpp=.o)

default: $(LOCAL_EXECUTABLE)

$(LOCAL_EXECUTABLE): $(LOCAL_OBJ)
	mkdir -p bin
	$(CC) $(LOCAL_OBJ) $(LOCAL_CFLAGS) $(LOCAL_LD_PATH) -o bin/$@
	mkdir -p $(INSTALL_ROOT)/bin
	cp bin/$(LOCAL_EXECUTABLE) $(INSTALL_ROOT)/bin/
	chmod 755 $(INSTALL_ROOT)/bin/$(EXECUTABLE)

.cpp.o:
	$(CC) $(LOCAL_LD_PATH) $(LOCAL_INCLUDES) $(LOCAL_CFLAGS) -c $< -o $@

clean:
	rm -rf $(LOCAL_EXECUTABLE)
	rm -rf $(LOCAL_OBJ)

