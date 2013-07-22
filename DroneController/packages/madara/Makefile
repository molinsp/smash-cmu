######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

ACE_ROOT=$(PACKAGES_ROOT)/ace/ACE_wrappers/build/$(TARGET)
MADARA_ROOT=$(PACKAGES_ROOT)/madara/madara
MADARA_TARGET_ROOT=$(MADARA_ROOT)/build/$(TARGET)

#================================
#BUILD MADARA
#================================
all:

	@echo "Starting libMADARA"


#Check if packages/madara exists
ifeq ($(wildcard $(MADARA_ROOT)),)
	@echo "Downloading MADARA source..."
	@svn checkout http://madara.googlecode.com/svn/trunk/ $(MADARA_ROOT)
else
	@echo "MADARA source already exists"
endif

#Create build folder host
ifeq ($(wildcard $(MADARA_TARGET_ROOT)),)
	@echo "Creating $(MADARA_TARGET_ROOT)"
	@mkdir -p $(MADARA_TARGET_ROOT)/lib
	@mkdir -p $(MADARA_TARGET_ROOT)/make
else
	@echo "$(MADARA_TARGET_ROOT) already created"
endif

#Generate makefiles
ifeq ($(wildcard $(MADARA_TARGET_ROOT)/make/GNUmakefile),)
	@echo "Generating makefiles"
	@cd $(MADARA_ROOT); ACE_ROOT=$(ACE_ROOT) MADARA_ROOT=$(MADARA_ROOT) perl $(ACE_ROOT)/bin/mwc.pl -type gnuace MADARA.mwc
else
	@echo "Using previously generated makefiles"
	@cp $(MADARA_TARGET_ROOT)/make/* $(MADARA_ROOT)
endif

ifeq ($(wildcard $(MADARA_TARGET_ROOT)/lib/libMADARA.so.*),)
	@echo "Building libMADARA"
	cd $(MADARA_ROOT); LD_LIBRARY_PATH=$(INSTALL_ROOT)/lib MADARA_ROOT=$(MADARA_ROOT) ACE_ROOT=$(ACE_ROOT) make cid=1 tests=1 -j $(JOBS)
else
	@echo "libMADARA already built"
	@rm -f $(MADARA_ROOT)/libMADARA*
	@cp $(MADARA_TARGET_ROOT)/lib/* $(MADARA_ROOT)
endif

ifneq ($(wildcard $(INSTALL_ROOT)/lib/libMADARA.so),)
	@echo "removing previous libMADARA from $(INSTALL_ROOT)"
	@rm $(INSTALL_ROOT)/lib/libMADARA*
endif

	@echo "Installing libMADARA.so"
	MADARA_ROOT=$(MADARA_ROOT) INSTALL_ROOT=$(INSTALL_ROOT) $(PACKAGES_ROOT)/madara/install.sh
	@mv $(MADARA_ROOT)/libMADARA.so.* $(MADARA_TARGET_ROOT)/lib
	@mv $(MADARA_ROOT)/GNUmakefile* $(MADARA_TARGET_ROOT)/make
	@rm -rf $(MADARA_ROOT)/.shobj $(MADARA_ROOT)/.depend* $(MADARA_ROOT)/GNUmakefile*
	@echo "Done with libMADARA"

#================================
#END MADARA
#================================

update: realclean
	cd madara; svn update

clean:
	rm -rf $(MADARA_ROOT)/.shobj $(MADARA_ROOT)/.obj $(MADARA_ROOT)/.depend* $(MADARA_ROOT)/GNUmakefile* $(MADARA_ROOT)/libMADARA* $(MADARA_ROOT)/test_* $(MADARA_ROOT)/tutorial_*

realclean: clean
	rm -rf $(MADARA_ROOT)/build
