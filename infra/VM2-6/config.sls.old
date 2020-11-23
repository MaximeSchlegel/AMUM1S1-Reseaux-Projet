# Configuration eth1 et et2
# RAPPEL: eth0 est à vagrant, ne pas y toucher

## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False
    
## Suppression de la passerelle par défaut
ip route del default:
  cmd:
    - run

## Configuration de VM2
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:1::26
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
    - ipv6ipaddr: fc00:1234:2::26
    - ipv6netmask: 64

## Routes toward LAN3-6 and LAN4-6
#routes:
#  network.routes:
#    - name: eth1
#    - routes:
#      - name: LAN3-6
#        ipaddr: fc00:1234:3::/64
#        gateway: fc00:1234:1::16
#    - name: eth2
#    - routes:
#      - name: LAN4-6
#        ipaddr: fc00:1234:4::/64
#        gateway: fc00:1234:2::36
## Ne marche pas avec plusieurs interfaces: donc j'utilise directement les commandes

sudo ip -6 route add fc00:1234:3::/64 via fc00:1234:1::16 dev eth1:
  cmd:
    - run

sudo ip -6 route add fc00:1234:4::/64 via fc00:1234:2::36 dev eth2:
  cmd:
    - run

## Enable ipv6 forwarding
net.ipv6.conf.all.forwarding:
  sysctl:
    - present
    - value: 1
