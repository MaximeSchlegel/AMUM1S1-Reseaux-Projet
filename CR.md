Maxime SCHLEGEL

# Projet Réseaux 2020

## 1. Configuration Réseau

### 1.1. Topologie et Adressage

J'ai choisit d'utiliser un adressage fixe pour ce projet. On a donc les adresses suivantes sur les VMs.
|        | LAN1            | LAN2            | LAN3-6           | LAN4-6           | LAN1-6           | LAN2-6           |
| ------ | --------------- | --------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| réseau | 172.16.2.128/28 | 172.16.2.160/28 | fc00:1234:3::/64 | fc00:1234:4::/64 | fc00:1234:1::/64 | fc00:1234:2::/64 |
| VM1    | 172.16.2.131    |                 | fc00:1234:3::1   |                  |                  |                  |
| VM2    | 172.16.2.132    | 172.16.2.162    |                  |                  |                  |                  |
| VM3    |                 | 172.16.2.163    |                  | fc00:1234:4::3   |                  |                  |
| VM1-6  |                 |                 | fc00:1234:3::16  |                  | fc00:1234:1::16  |                  |
| VM2-6  |                 |                 |                  |                  | fc00:1234:1::26  | fc00:1234:2::26  |
| VM3-6  |                 |                 |                  | fc00:1234:4::36  |                  | fc00:1234:2::36  |


Bien que la configuration salt rencontre des problème lors de l'installation des package (n'arrive pas à trouver les repo), une fois qu'elles ont reussit on arrive bien à 
Une fois la configuration avec salt terminé on peut bien accéder aux serveurs echo de la VM3 et VM3-6.


## 2. L'interface virtuelle TUN

### 2.2. Configuration de l'interface

Pour configurer l'interface tun0, j'ai choisit d'utiliser un masque en /64. Et j'ai aussi activé le "packet forwarding" sur la VM pour lui permettre de retransmettre les packets provenant de extérieurs sur cette interface.

Il faudra modifier la route pour accéder au LAN4-6: passer la paserelle de VM2-6 à l'interface eth2 de la VM1. 

```
> ping6 fc00:1234:ffff::1

No.     Time           Source                Destination           Protocol Length Info
      1 0.000000000    fc00:1234:ffff::1     fc00:1234:ffff::1     ICMPv6   120    Echo (ping) request id=0x074f, seq=1, hop limit=64 (reply in 2)

Frame 1: 120 bytes on wire (960 bits), 120 bytes captured (960 bits) on interface 0
Linux cooked capture
Internet Protocol Version 6, Src: fc00:1234:ffff::1 (fc00:1234:ffff::1), Dst: fc00:1234:ffff::1 (fc00:1234:ffff::1)
Internet Control Message Protocol v6

No.     Time           Source                Destination           Protocol Length Info
      2 0.000007000    fc00:1234:ffff::1     fc00:1234:ffff::1     ICMPv6   120    Echo (ping) reply id=0x074f, seq=1, hop limit=64 (request in 1)

Frame 2: 120 bytes on wire (960 bits), 120 bytes captured (960 bits) on interface 0
Linux cooked capture
Internet Protocol Version 6, Src: fc00:1234:ffff::1 (fc00:1234:ffff::1), Dst: fc00:1234:ffff::1 (fc00:1234:ffff::1)
Internet Control Message Protocol v6
```

```
>ping6 fc00:1234:ffff::10
No.     Time           Source                Destination           Protocol Length Info
      1 0.000000000    fc00:1234:ffff::1     fc00:1234:ffff::10    ICMPv6   120    Echo (ping) request id=0x0775, seq=1, hop limit=64 (no response found!)

Frame 1: 120 bytes on wire (960 bits), 120 bytes captured (960 bits) on interface 0
Linux cooked capture
Internet Protocol Version 6, Src: fc00:1234:ffff::1 (fc00:1234:ffff::1), Dst: fc00:1234:ffff::10 (fc00:1234:ffff::10)
Internet Control Message Protocol v6
```

Lorsque l'on ping l'ip ```fc00:1234:ffff::1```, on obtient bien une réponse ce qui n'est pas le cas pour l'interface ```fc00:1234:ffff::10```.
Ce comportemant correspond à celui attendu: l'interface même virtuelle reste une interface systeme elle répond donc normalemet aux pings. Comme il n'y a pas d'interface derrière l'ip ```fc00:1234:ffff::10``` nous n'avons pas deréponse.

### 2.3. Récupération des paquets

_4:_
>IFF_NO_The PI tells the kernel that it does not need to provide message    information, that is, it only needs to provide "pure" IP messages and no other bytes.Otherwise (do not set IFF_NO_PI), which adds four extra bytes >>(2-byte identity and 2-byte protocol) at the beginning of the message.

## 3. Un tunnel simple pour IPv6

### 3.1. Redirection du trafic entrant
