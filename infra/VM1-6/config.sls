# Configuration eth1 et eth2
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

##Configuration de VM1-6 interface
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:1::16
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
    - ipv6ipaddr: fc00:1234:3::16
    - ipv6netmask: 64

## Configuration de la route vers LAN2-6 et 4-6 via VM2
routes:
  network.routes:
    - name: eth1
    - routes:
    #   - name: LAN2-6
    #     ipaddr: fc00:1234:2::/64
    #     gateway: fc00:1234:1::26
      - name: LAN4-6
        ipaddr: fc00:1234:4::/64
        gateway: fc00:1234:3::1
      
## Enable IPv6 forwarding
net.ipv6.conf.all.forwarding:
  sysctl:
    - present
    - value: 1