######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################



#All of the following values need populated. Building has not been tested with
#any toolchains other than the the default one represented below

TARGET=arm
PLATFORM=AR_DRONE_2
LOCAL_CROSS=$(HOME_DIR)/toolchain/arm-none-linux-gnueabi
LOCAL_CROSS_PREFIX=$(LOCAL_CROSS)/bin/arm-none-linux-gnueabi-
LOCAL_GCC=$(LOCAL_CROSS_PREFIX)gcc
LOCAL_GPLUSPLUS=$(LOCAL_CROSS_PREFIX)g++


