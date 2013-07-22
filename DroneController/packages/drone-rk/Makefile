######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

DRK_ROOT=$(PACKAGES_ROOT)/drone-rk/drone-rk

#================================
#BUILD DRK
#================================
all: download

	@echo "Starting libdrk"

#Check if packages/drone-rk/drone-rk exists
ifeq ($(wildcard $(DRK_ROOT)/src/include/libdrk.so),)
	@echo "Building libdrk"
	cd $(DRK_ROOT)/src/include; make -j $(JOBS)
else
	@echo "libdrk already built"
endif

	@echo "copying libdrk and include files to install dir"
	@cp $(DRK_ROOT)/src/include/libdrk.so $(INSTALL_ROOT)/lib
	@cp $(DRK_ROOT)/src/include/*.h $(INSTALL_ROOT)/include
	@cp $(DRK_ROOT)/src/libs/drk-api/*.h $(INSTALL_ROOT)/include
	@cp $(DRK_ROOT)/src/libs/drk_sched/*.h $(INSTALL_ROOT)/include

#================================
#DOWNLOAD DRK
#================================
download:
ifeq ($(wildcard $(DRK_ROOT)),)
	@echo "Downloading Drone-RK source and compilers..."
	@git clone ssh://drone-rk@wise.ece.cmu.edu/~/drone-rk.git $(DRK_ROOT)
else
	@echo "Drone-RK source already exists"
endif

#================================
#END DRK
#================================

update: realclean
	cd drone-rk; git pull

clean:
	make -C $(DRK_ROOT)/src/include clean

realclean: clean
