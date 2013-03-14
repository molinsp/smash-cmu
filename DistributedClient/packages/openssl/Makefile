######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

CC_ROOT=$(HOME_DIR)/toolchain/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-

all:

	@echo "Starting OpenSSL"

ifeq ($(wildcard openssl),)
	@git clone git://git.openssl.org/openssl.git
endif

ifeq ($(wildcard openssl/libssl.so),)
	@echo "Building libssl"
	@cd openssl; CC=$(CC_ROOT)gcc RANLIB=$(CC_ROOT)ranlib ./Configure linux-armv4 --prefix=$(INSTALL_ROOT) --shared
	@cd openssl; make
else
	@echo "libssl already built"
endif

ifeq ($(wildcard $(INSTALL_ROOT)/lib/libssl.so),)
	@echo "Installing OpenSSL"
	@cd openssl; make install
else
	@echo "OpenSSL already installed"
endif

	@echo "Done with OpenSSL"
