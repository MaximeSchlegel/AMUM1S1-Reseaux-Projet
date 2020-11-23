# Configuration eth1 et et2
# RAPPEL: eth0 est à vagrant, ne pas y toucher

## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False
    
## Configuration de VM2
## "Broke" the VM => no connections
eth1:
  network.managed:
    - enabled: False

eth2:
  network.managed:
    - enabled: False