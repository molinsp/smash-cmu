######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

INSTALL_PREFIX=$(INSTALL_ROOT)
no_hidden_visibility=1
OCFLAGS=-Os
include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU
