#!/bin/bash
#$1 : radvd config file to install 

cp $1 /etc/radvd.conf
chmod 664 /etc/radvd.conf
sudo service radvd restart