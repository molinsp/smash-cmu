#!/bin/bash

#################################################################
# @author James Edmondson <jedmondson@gmail.com>
# this script will compile the command line system controller. Be sure
# to set the following environment variables:
#
# $MADARA_CLIENT_ROOT   this is the location of the madara_client package dir
# $MADARA_ROOT          this is the location of your MADARA installation dir
# $ACE_ROOT             this is the location of your ACE installation dir
################################################################

cd $MADARA_CLIENT_ROOT/../../../CommandLineSystemController
g++ -I$MADARA_ROOT/include -I$ACE_ROOT -I$MADARA_CLIENT_ROOT -L$MADARA_ROOT/lib -L$ACE_ROOT/lib simple_coverage_test.cpp -lMADARA -lACE -o simple_coverage_test

