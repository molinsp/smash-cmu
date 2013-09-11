######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

LOCAL_SOURCES += platforms/ar_drone_2/platform_ardrone2.cpp
LOCAL_SOURCES += platforms/comm/kb_setup.cpp
LOCAL_SOURCES += platforms/comm/broadcast/comm_broadcast.cpp

LOCAL_CFLAGS += -ldrk
