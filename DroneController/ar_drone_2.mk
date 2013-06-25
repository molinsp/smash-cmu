######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

TARGET=arm
#PLATFORM=AR_DRONE_2
LOCAL_CROSS=~/toolchains/arm-2009q1
LOCAL_CROSS_PREFIX=$(LOCAL_CROSS)/bin/arm-none-linux-gnueabi-
LOCAL_GCC=$(LOCAL_CROSS_PREFIX)gcc
LOCAL_GPLUSPLUS=$(LOCAL_CROSS_PREFIX)g++


