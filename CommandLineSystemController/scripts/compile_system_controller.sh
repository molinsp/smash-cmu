#!/bin/bash

cd $MADARA_CLIENT_ROOT/../../../CommandLineSystemController
g++ -I$MADARA_ROOT/include -I$ACE_ROOT -I$MADARA_CLIENT_ROOT -L$MADARA_ROOT/lib -L$ACE_ROOT/lib simple_coverage_test.cpp -lMADARA -lACE -o simple_coverage_test

