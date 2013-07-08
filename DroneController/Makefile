######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

#GENERAL SETUP
HOME_DIR:=$(CURDIR)
PACKAGES_ROOT:=$(HOME_DIR)/packages
INSTALL_ROOT:=$(HOME_DIR)/install
JOBS:=6

LOCAL_CMD = TARGET=$(TARGET) \
	PLATFORM=$(PLATFORM) \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	LOCAL_CROSS=$(LOCAL_CROSS) \
	LOCAL_CROSS_PREFIX=$(LOCAL_CROSS_PREFIX) \
	LOCAL_GCC=$(LOCAL_GCC) \
	LOCAL_GPLUSPLUS=$(LOCAL_GPLUSPLUS) \
	make

VREP_CMD := TARGET=LINUX \
	PLATFORM=LINUX \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	LOCAL_GCC=gcc \
	LOCAL_GPLUSPLUS=g++ \
	JOBS=$(JOBS) \
	make

AR_DRONE_2_CMD := TARGET=AR_DRONE_2 \
	PLATFORM=AR_DRONE_2 \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	LOCAL_CROSS_PREFIX=$(HOME)/dev/drone-rk/2toolchain/arm-2010.09/bin/arm-none-linux-gnueabi- \
	LOCAL_GCC=$(LOCAL_CROSS_PREFIX)gcc \
	LOCAL_GPLUSPLUS=$(LOCAL_CROSS_PREFIX)g++ \
	JOBS=$(JOBS) \
	make

.make_install_dir:
	rm -rf $(INSTALL_ROOT)
	mkdir -p $(INSTALL_ROOT)/bin
	mkdir -p $(INSTALL_ROOT)/lib
	mkdir -p $(INSTALL_ROOT)/include
	mkdir -p $(INSTALL_ROOT)/doc
	mkdir -p $(INSTALL_ROOT)/share

vrep: .make_install_dir
	cd $(PACKAGES_ROOT)/ace; $(VREP_CMD)
	cd $(PACKAGES_ROOT)/madara; $(VREP_CMD)
	cd $(PACKAGES_ROOT)/madara_client; $(VREP_CMD)

ar_drone_2: .make_install_dir
	cd $(PACKAGES_ROOT)/ace; $(AR_DRONE_2_CMD)
	cd $(PACKAGES_ROOT)/madara; $(AR_DRONE_2_CMD)
	@echo "ar_drone_2 not completely working"

%:
	@cd $(PACKAGES_ROOT)/$@; $(LOCAL_CMD)

%-clean:
	@cd $(PACKAGES_ROOT)/$*; $(LOCAL_CMD) clean

%-realclean:
	@cd $(PACKAGES_ROOT)/$*; $(LOCAL_CMD) realclean
	
clean:
	for d in `ls packages`; do make $$d-clean; done

realclean:
	rm -rf install
	for d in `ls packages`; do make $$d-realclean; done
