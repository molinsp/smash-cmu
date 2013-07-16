#!/bin/sh

######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################


# create needed directories 
mkdir -p $INSTALL_ROOT/include
mkdir -p $INSTALL_ROOT/lib
mkdir -p $INSTALL_ROOT/doc/madara
mkdir -p $MADARA_ROOT/install_tmp

# copy the MADARA and ACE source includes into the appropriate directory
cp -r $MADARA_ROOT/include/madara $MADARA_ROOT/install_tmp

# remove any shared objects, symbolic links and svn from the include directory
#echo "Removing all symbolic links from source tree"
find $MADARA_ROOT/install_tmp -type l -exec rm -f {} \;
#echo "Removing all shared objects from source tree"
find $MADARA_ROOT/install_tmp -name "*.so*" -type f -exec rm -f {} \;
#echo "Removing all SVN directories from source tree"
find $MADARA_ROOT/install_tmp -name .svn -type d -exec rm -rf {} 2> /dev/null \;
#find $MADARA_ROOT/install_tmp -name "*.cpp" -type f -exec rm -f {} \;
cp -r $MADARA_ROOT/install_tmp/* $INSTALL_ROOT/include
rm -rf $MADARA_ROOT/install_tmp

# update changelog with the last 10 entries from the repository
svn log http://madara.googlecode.com/svn/trunk/ -l 10 > $INSTALL_ROOT/doc/madara/changelog

# merge MADARA and ACE copyright and license information
cat $MADARA_ROOT/LICENSE.txt >> $INSTALL_ROOT/doc/madara/copyright

cp $MADARA_ROOT/libMADARA.so.* $INSTALL_ROOT/lib
cp $MADARA_ROOT/test_reasoning_throughput $INSTALL_ROOT/bin
cp $MADARA_ROOT/test_broadcast $INSTALL_ROOT/bin

ln -s `ls $INSTALL_ROOT/lib | grep "libMADARA.so.*"` $INSTALL_ROOT/lib/libMADARA.so
