######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

CC_ROOT=$(HOME_DIR)/toolchain/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-

all:

	@echo "Starting wpa_supplicant"

ifeq ($(wildcard wpa_supplicant),)
	@wget http://hostap.epitest.fi/releases/wpa_supplicant-1.1.tar.gz
	@tar -xf wpa_supplicant-1.1.tar.gz
	@mv wpa_supplicant-1.1 wpa_supplicant
endif

ifeq ($(wildcard wpa_supplicant/wpa_supplicant/.config),)
	@echo "Creating .config"
	@echo "CONFIG_DRIVER_HOST=y" > wpa_supplicant/wpa_supplicant/.config
	@echo "CONFIG_DRIVER_WEXT=y" >> wpa_supplicant/wpa_supplicant/.config
	@echo "CONFIG_WIRELESS_EXTENSION=y" >> wpa_supplicant/wpa_supplicant/.config
	@echo "CFLAGS += -I$(INSTALL_ROOT)/include" >> wpa_supplicant/wpa_supplicant/.config
	@echo "CC=$(CC_ROOT)gcc -L$(INSTALL_ROOT)/lib" >> wpa_supplicant/wpa_supplicant/.config
else
	@echo ".config already created"
endif

ifeq ($(wildcard wpa_supplicant/wpa_supplicant/wpa_supplicant),)
	@echo "Building wpa_supplicant"
	@cd wpa_supplicant/wpa_supplicant; make
else
	@echo "wpa_supplicant already built"
endif

ifeq ($(wildcard $(INSTALL_ROOT)/bin/wpa_supplicant),)
	@echo "Installing wpa_supplicant"
	@cp wpa_supplicant/wpa_supplicant/wpa_supplicant $(INSTALL_ROOT)/bin/wpa_supplicant
	@cp wpa_supplicant/wpa_supplicant/wpa_passphrase $(INSTALL_ROOT)/bin/wpa_passphrase
else
	@echo "wpa_supplicant already installed"
endif

	@echo "Done with wpa_supplicant"
