# Configuration eth1 et eth2
# RAPPEL: eth0 est à vagrant, ne pas y toucher

## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False

## Install radvd for upgrade
radvd:
  pkg:
    - installed

## Suppression de la passerelle par défaut
ip route del default:
  cmd:
    - run

##Configuration de VM1
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - ipaddr: 172.16.2.131
    - netmask: 28
    # - gateway: 172.16.2.132 # Gateway as all the IPv4 that is not directly acessible need to be routed throught VM2

eth2:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:3::1
    - ipv6netmask: 64
    # - ipv6gateway: fc00:1234:3::16 # Gateway as all the IPv6 that is not directly acessible need to be routed throught VM1-6


## Configuration de la route vers LAN2, LAN1-6, LAN2-6 et 4-6 via VM2
routesEth1:
 network.routes:
   - name: eth1
   - routes:
     - name: LAN2
       ipaddr: 172.16.2.160/28
       gateway: 172.16.2.132

routesEth2:
   network.routes:
     - name: eth2
     - routes:
       - name: LAN1-6
         ipaddr: fc00:1234:1::/64
         gateway: fc00:1234:3::16
       - name: LAN2-6
         ipaddr: fc00:1234:2::/64
         gateway: fc00:1234:3::16
       - name: LAN4-6
         ipaddr: fc00:1234:4::/64
         gateway: fc00:1234:3::16