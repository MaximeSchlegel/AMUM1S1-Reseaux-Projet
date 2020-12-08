#!/bin/bash
# $1: name of the tun interface to configure
# $2: ip of the interface

ip -6 link set $1 up
ip -6 addr add $2 dev $1
sysctl -w net.ipv6.conf.all.forwarding=1