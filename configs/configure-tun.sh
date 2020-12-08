#!/bin/bash
# $1: name of the tun interface to configure
# $2: IP of the tun interface
# $3: LAN accessible throught the tun 


ip -6 link set $1 up
ip -6 addr add $2"/64" dev $1

sysctl -q -w net.ipv6.conf.all.forwarding=1

ip -6 route add $3 via $2"0" dev $1
