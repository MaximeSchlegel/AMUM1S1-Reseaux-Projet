#!/bin/bash
# $1: name of the tun interface to configure
# $2: ip of the iterface

ip -6 link set $1 up
ip -6 addr add fc00:1234:ffff::1 dev $1
