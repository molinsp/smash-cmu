######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

ACE_ROOT=$(PACKAGES_ROOT)/ace/ACE_wrappers/build/$(TARGET)
MADARA_ROOT=$(PACKAGES_ROOT)/madara/madara

#================================
#BUILD MADARA
#================================
all:

	@echo "Starting libMADARA"


#Check if packages/madara exists
ifeq ($(wildcard $(MADARA_ROOT)),)
	@echo "You must download the MADARA source, press enter to do this now..."
	@svn checkout http://madara.googlecode.com/svn/trunk/ $(MADARA_ROOT)
else
	@echo "MADARA source already exists"
endif

#Generate makefiles
ifeq ($(wildcard $(MADARA_ROOT)/GNUmakefile),)
	@echo "Generating makefiles"
	@cd $(MADARA_ROOT); ACE_ROOT=$(ACE_ROOT) MADARA_ROOT=$(MADARA_ROOT) perl $(ACE_ROOT)/bin/mwc.pl -type gnuace MADARA.mwc
else
	@echo "Makefiles already generated"
endif

ifeq ($(wildcard $(MADARA_ROOT)/libMADARA.so),)
	@echo "Building libMADARA"
	cd $(MADARA_ROOT); LD_LIBRARY_PATH=$(INSTALL_ROOT)/lib MADARA_ROOT=$(MADARA_ROOT) ACE_ROOT=$(ACE_ROOT) make cid=1 tests=0
else
	@echo "libMADARA already built"
endif

ifeq ($(wildcard $(INSTALL_ROOT)/lib/libMADARA.so),)
	@echo "Installing libMADARA.so"
	MADARA_ROOT=$(MADARA_ROOT) INSTALL_ROOT=$(INSTALL_ROOT) $(PACKAGES_ROOT)/madara/install.sh
else
	@echo "libMADARA.so already installed"
endif
	@echo "Done with libMADARA"

#================================
#END MADARA
#================================
