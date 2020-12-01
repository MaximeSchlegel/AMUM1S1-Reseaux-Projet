#!/bin/bash

ip -6 link set $1 up
ip -6 addr add fc00:1234:ffff::1/64 dev $1