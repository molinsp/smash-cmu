######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

#GENERAL SETUP
HOME_DIR=$(CURDIR)
PACKAGES_ROOT=$(HOME_DIR)/packages
INSTALL_ROOT=$(HOME_DIR)/install

include toolchain.mk

LOCAL_CMD = 	TARGET=$(TARGET) \
		PLATFORM=$(PLATFORM) \
		HOME_DIR=$(HOME_DIR) \
		PACKAGES_ROOT=$(PACKAGES_ROOT) \
		INSTALL_ROOT=$(INSTALL_ROOT) \
		LOCAL_CROSS=$(LOCAL_CROSS) \
		LOCAL_CROSS_PREFIX=$(LOCAL_CROSS_PREFIX) \
		LOCAL_GCC=$(LOCAL_GCC) \
		LOCAL_GPLUSPLUS=$(LOCAL_GPLUSPLUS) \
		make

%:
	@cd $(PACKAGES_ROOT)/$@; $(LOCAL_CMD)

%-clean:
	@cd $(PACKAGES_ROOT)/$*; $(LOCAL_CMD) clean
