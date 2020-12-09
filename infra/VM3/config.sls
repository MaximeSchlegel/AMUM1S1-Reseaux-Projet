# Configuration eth1 et eth2
# RAPPEL: eth0 est à vagrant, ne pas y toucher


## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False

## Installation de radvd 
radvd:
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

## Configuration de VM3
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - ipaddr: 172.16.2.163
    - netmask: 28

eth2:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:4::3
    - ipv6netmask: 64
    # - ipv6gateway: fc00:1234:4::36 # Gateway as all the IPv6 that is not directly acessible need to be routed throught VM3-6

## Configuration de la route vers LAN1, LAN1-6, LAN2-6 et 3-6 via VM2
routesEth1:
 network.routes:
   - name: eth1
   - routes:
     - name: LAN1
       ipaddr: 172.16.2.128/28
       gateway: 172.16.2.162

routesEth2:
  network.routes:
   - name: eth2
   - routes:
     - name: LAN1-6
       ipaddr: fc00:1234:1::/64
       gateway: fc00:1234:4::36
     - name: LAN2-6
       ipaddr: fc00:1234:2::/64
       gateway: fc00:1234:4::36
     - name: LAN3-6
       ipaddr: fc00:1234:3::/64
       gateway: fc00:1234:4::36
