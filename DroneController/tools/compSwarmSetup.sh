#!/bin/sh
if [ $# != 3 ]
then
    echo "usage: $0 <ADAPTOR> <SSID> <ADDRESS>"
    echo "Sets the wireless adaptor to ad hoc mode with SSID and address"
else
    sudo ip link set $1 down; sudo iwconfig $1 mode ad-hoc; sudo iwconfig $1 essid $2; sudo ifconfig $1 $3; sudo ip link set $1 up
fi
