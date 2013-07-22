######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

#ACE SETUP
ACE_HOME=$(PACKAGES_ROOT)/ace
ACE_ROOT=$(ACE_HOME)/ACE_wrappers

#ACE TARGET SETUP
ACE_TARGET_FOLDER=build/$(TARGET)
ACE_TARGET_ROOT=$(ACE_ROOT)/$(ACE_TARGET_FOLDER)
ACE_TARGET_CONFIG_H=$(ACE_TARGET_ROOT)/ace/config.h
ACE_TARGET_GNU=$(ACE_TARGET_ROOT)/include/makeinclude/platform_$(TARGET).GNU
ACE_TARGET_PLATFORM_MACROS_GNU=$(ACE_TARGET_ROOT)/include/makeinclude/platform_macros.GNU
#END ACE SETUP

#================================
#BUILD ACE
#================================

all: 

#================================
#SETUP ACE DIRECTORY
#================================

#Check if packages/ace/ACE_wrappers exists
ifeq ($(wildcard $(ACE_ROOT)),)
	@echo "Downloading ACE Source"
	@svn co svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE+TAO+CIAO $(ACE_HOME)
else
	@echo "ACE source already exists"
endif

#================================
#END ACE SETUP
#================================

#================================
#BUILD ACE
#================================

	@echo "Starting $(ACE_TARGET_FOLDER)"

#Create build folder host
ifeq ($(wildcard $(ACE_TARGET_ROOT)),)
	@echo "Creating $(ACE_TARGET_FOLDER)"
	@mkdir -p $(ACE_TARGET_ROOT)
else
	@echo "$(ACE_TARGET_FOLDER) already created"
endif

#Check if ace has been moved in to the ACE_HOST_FOLDER
ifeq ($(wildcard $(ACE_TARGET_ROOT)/ace),)
	@echo "Symlinking $(ACE_TARGET_FOLDER)"
	@cd $(ACE_ROOT); ./bin/create_ace_build $(ACE_TARGET_FOLDER)
else
	@echo "$(ACE_TARGET_FOLDER) already simlinked"
endif

ifeq ($(wildcard $(ACE_TARGET_ROOT)/ace/GNUmakefile),)
	@echo "Generating makefiles"
	cd $(ACE_TARGET_ROOT); ACE_ROOT=$(ACE_TARGET_ROOT) $(ACE_TARGET_ROOT)/bin/mwc.pl -type gnuace ace/ace.mwc
else
	@echo "Makefiles already created"
endif

#check if ACE_HOST_FOLDER/ace/config.h exists
ifeq ($(wildcard $(ACE_TARGET_CONFIG_H)),)
	@echo "Creating config.h"
	@cat platforms/$(PLATFORM)_config.h > $(ACE_TARGET_CONFIG_H)
#	@echo '#include "ace/config-linux.h"' > $(ACE_TARGET_CONFIG_H)
else
	@echo "config.h already created"
endif

#Setup platform_arm.GNU
#ifeq ($(wildcard $(ACE_TARGET_GNU)),)
#	@echo "Creating platform_$(TARGET).GNU"
#	@echo "ARM=1" > $(ACE_TARGET_GNU)
#	@echo "CROSS_COMPILE=$(LOCAL_CROSS_PREFIX)" > $(ACE_TARGET_GNU)
#	@echo "INSTALL_PREFIX=$(INSTALL_ROOT)" >> $(ACE_TARGET_GNU)
#	@echo "no_hidden_visibility=1" >> $(ACE_TARGET_GNU)
#	@echo "OCFLAGS=-Os" >> $(ACE_TARGET_GNU)
#	@echo "include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU" >> $(ACE_TARGET_GNU)
#else
#	@echo "platform_$(TARGET).GNU already exists"
#endif

#Setup platform_macros.GNU
ifeq ($(wildcard $(ACE_TARGET_PLATFORM_MACROS_GNU)),)
	@echo "Creating platform_macros.GNU"
	@cat platforms/$(PLATFORM)_macros.GNU >  $(ACE_TARGET_PLATFORM_MACROS_GNU)
#	@echo 'include $(ACE_TARGET_ROOT)/include/makeinclude/platform_$(TARGET).GNU' > $(ACE_TARGET_PLATFORM_MACROS_GNU)
#	@echo "INSTALL_PREFIX=$(INSTALL_ROOT)" >> $(ACE_TARGET_PLATFORM_MACROS_GNU)
else
	@echo "platform_macros.GNU already created"
endif

ifeq ($(wildcard $(ACE_TARGET_ROOT)/lib/libACE.so),)
	@echo "Building libACE in $(ACE_TARGET_FOLDER)"
	ACE_ROOT=$(ACE_TARGET_ROOT) \
	make -C $(ACE_TARGET_ROOT)/ace -j $(JOBS)
else
	@echo "libACE already built"
endif

#Install ACE
#Must CD to directory because the ace files use PWD which is set to our current directory not ACE_ROOT
ifneq ($(wildcard $(INSTALL_ROOT)/lib/libACE.so),)
	@echo "removing previous libACE from $(INSTALL_ROOT)"
	@rm $(INSTALL_ROOT)/lib/libACE*
endif

	@echo "Installing libACE"
	@mkdir -p $(INSTALL_ROOT)/share/ace
	@cd $(ACE_TARGET_ROOT)/ace; \
	ACE_ROOT=$(ACE_TARGET_ROOT) \
	make -j $(JOBS) install

	@echo "Done in $(ACE_TARGET_FOLDER)"

#================================
#END BUILD TARGET
#================================

update: realclean
	cd ACE_wrappers; svn up

#================================
#END ACE
#================================
clean:
	for d in "$(ACE_ROOT)/build/"*; do make -C $$d/ace clean; done

realclean:
	rm -rf $(ACE_ROOT)/build
