######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

CC = $(LOCAL_GPLUSPLUS)

EXECUTABLE = opencv_test
SOURCES = images_test.cpp
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lACE

all:
	@$(CC) -L$(INSTALL_ROOT)/lib -I$(INSTALL_ROOT)/include -o $(EXECUTABLE) $(SOURCES) $(LIBS)
	@mkdir -p $(INSTALL_ROOT)/bin
	@cp $(EXECUTABLE) $(INSTALL_ROOT)/bin
	@chmod 755 $(INSTALL_ROOT)/bin/$(EXECUTABLE)

clean:

realclean: clean
