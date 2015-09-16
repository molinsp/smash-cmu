# Introduction #

This page describes the process of building and running a simple system controller that sets area coverage parameters.


# Details #

**(a) Download smash-cmu repository.**

**(b) Compile ACE and MADARA for Linux**

This can be easily done my using the v\_rep target to issue make command from inside DroneController folder. Both of the compiled libraries will be placed inside DroneController/install/lib directory. It needs to be copied to somewhere else so that it doesn't get rebuild for arm when the DroneController is compiled using ar\_drone\_2 target.

**(c) Compile system controller with the following command:**

cd smash-cmu/CommandLineSystemController

g++ -I$MADARA\_ROOT/include -I$ACE\_ROOT -I$MADARA\_CLIENT\_ROOT -L<Where ever MADARA library was stored in step (b)> -L$<Where ever ACE library was stored in step (b)> simple\_coverage\_test.cpp -lMADARA -lACE -o simple\_coverage\_test

**(d) Run the system controller**

cd smash-cmu/CommandLineSystemController

./simple\_coverage\_test -i <SYSTEM CONTROLLER ID> -d <NUMBER OF DRONES> -n <NORTHERN LAT> -s <SOUTHERN LAT> -w <WESTERN LAT> -e <EASTERN LAT>