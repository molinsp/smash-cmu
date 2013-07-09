######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

V_REP_CMD = TARGET=V_REP \
	PLATFORM=V_REP \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	LOCAL_GCC=gcc \
	LOCAL_GPLUSPLUS=g++ \
	JOBS=$(JOBS) \
	make
