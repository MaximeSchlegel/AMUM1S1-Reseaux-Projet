#!/bin/bash

ip -6 link set $1 up
ip -6 addr add $2 dev $1

sysctl -w net.ipv6.conf.all.forwarding=1

