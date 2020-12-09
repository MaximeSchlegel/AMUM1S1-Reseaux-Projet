# Configuration eth1
# RAPPEL: eth0 est à vagrant, ne pas y toucher

## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False

## Installation de iperf3
iperf3:
  pkg:
    - installed
## Installation de netcat6
netcat6:
  pkg:
    - installed

## Add echo server
inetutils-inetd:
  pkg:
    - installed
  service:
    - running
    - enable: True

sudo update-inetd --add "echo stream tcp6 nowait nobody internal":
  cmd:
    - run

service inetutils-inetd start:
  cmd:
    - run

sudo service inetutils-inetd restart:
  cmd:
    - run

## Suppression de la passerelle par défaut
ip route del default:
  cmd:
    - run

##Configuration de VM3
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:2::36
    - ipv6netmask: 64

eth2:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:4::36
    - ipv6netmask: 64

## Configuration de la route vers LAN1-6 and LAN3-6 via VM2
routes:
  network.routes:
    - name: eth1
    - routes:
      - name: LAN1-6
        ipaddr: fc00:1234:1::/64
        gateway: fc00:1234:2::26
      - name: LAN3-6
        ipaddr: fc00:1234:3::/64
        gateway: fc00:1234:2::26

## Enable IPv6 forwarding
net.ipv6.conf.all.forwarding:
  sysctl:
    - present
    - value: 1
