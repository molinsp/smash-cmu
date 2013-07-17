######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

LOCAL_CROSS_PREFIX:=$(HOME)/dev/drone-rk/2toolchain/arm-2010.09/bin/arm-none-linux-gnueabi-

AR_DRONE_2_CMD = TARGET=AR_DRONE_2 \
	LOCAL_CROSS_PREFIX=$(LOCAL_CROSS_PREFIX) \
	PLATFORM=AR_DRONE_2 \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	LOCAL_GCC=$(LOCAL_CROSS_PREFIX)gcc \
	LOCAL_GPLUSPLUS=$(LOCAL_CROSS_PREFIX)g++ \
	JOBS=$(JOBS) \
	make
