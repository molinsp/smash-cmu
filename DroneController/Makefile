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

include v_rep.mk
include ar_drone_2.mk

LOCAL_CMD = PLATFORM=$(PLATFORM) \
	HOME_DIR=$(HOME_DIR) \
	PACKAGES_ROOT=$(PACKAGES_ROOT) \
	INSTALL_ROOT=$(INSTALL_ROOT) \
	make

.make_install_dir:
	rm -rf $(INSTALL_ROOT)
	mkdir -p $(INSTALL_ROOT)/bin
	mkdir -p $(INSTALL_ROOT)/lib
	mkdir -p $(INSTALL_ROOT)/include
	mkdir -p $(INSTALL_ROOT)/doc
	mkdir -p $(INSTALL_ROOT)/share

v_rep: .make_install_dir
	cd $(PACKAGES_ROOT)/ace; $(V_REP_CMD)
	cd $(PACKAGES_ROOT)/madara; $(V_REP_CMD)
	make madara_client-realclean # hack to force recompile, should be better way to do this
	cd $(PACKAGES_ROOT)/madara_client; $(V_REP_CMD)

ar_drone_2: .make_install_dir
	cd $(PACKAGES_ROOT)/drone-rk; $(AR_DRONE_2_CMD)
	cd $(PACKAGES_ROOT)/ace; $(AR_DRONE_2_CMD)
	cd $(PACKAGES_ROOT)/madara; $(AR_DRONE_2_CMD)
	make madara_client-realclean # hack to force recompile, should be better way to do this
	cd $(PACKAGES_ROOT)/madara_client; $(AR_DRONE_2_CMD)

update:
	for d in `ls packages`; do make $$d-update; done

%-update:
	@cd $(PACKAGES_ROOT)/$*; $(LOCAL_CMD) update

#%:
#	@cd $(PACKAGES_ROOT)/$@; make

%-clean:
	@cd $(PACKAGES_ROOT)/$*; make clean

%-realclean:
	@cd $(PACKAGES_ROOT)/$*; PACKAGES_ROOT=`pwd`/.. make realclean
	
clean:
	for d in `ls packages`; do make $$d-clean; done

realclean:
	rm -rf install
	for d in `ls packages`; do make $$d-realclean; done
