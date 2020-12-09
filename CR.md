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

Lorsque l'on ping ```fc00:1234:ffff::1``` on obtient une réponse par contre on ne le voit pas sur la sortie standard.

Inversement  losque l'on ping ```fc00:1234:ffff::10```, on obtient aucune réponse mais on peut l'observer sur la sortie standard.
Avec hexdump:
```
00000090  00 00 00 00 00 00 00 02  60 00 00 00 00 40 3a 40  |........`....@:@|
000000a0  fc 00 12 34 ff ff 00 00  00 00 00 00 00 00 00 01  |...4............|
000000b0  fc 00 12 34 ff ff 00 00  00 00 00 00 00 00 00 10  |...4............|
000000c0  80 00 40 a2 28 54 00 01  af 3b d0 5f 00 00 00 00  |..@.(T...;._....|
000000d0  bb a4 00 00 00 00 00 00  10 11 12 13 14 15 16 17  |................|
000000e0  18 19 1a 1b 1c 1d 1e 1f  20 21 22 23 24 25 26 27  |........ !"#$%&'|
000000f0  28 29 2a 2b 2c 2d 2e 2f  30 31 32 33 34 35 36 37  |()*+,-./01234567|
```

On observe ce comportement pour la seconde ip car l'interface virtuelle écrit les données dans le fichier de tun0 pour que le système puisse le lire.

Avec wireshark:
```
00000000  00 04 ff fe 00 00 00 00  00 00 00 00 00 00 86 dd  |................|
00000010  60 00 00 00 00 40 3a 40  fc 00 12 34 ff ff 00 00  |`....@:@...4....|
00000020  00 00 00 00 00 00 00 01  fc 00 12 34 ff ff 00 00  |...........4....|
00000030  00 00 00 00 00 00 00 10  80 00 40 a2 28 54 00 01  |..........@.(T..|
00000040  af 3b d0 5f 00 00 00 00  bb a4 00 00 00 00 00 00  |.;._............|
00000050  10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
00000060  20 21 22 23 24 25 26 27  28 29 2a 2b 2c 2d 2e 2f  | !"#$%&'()*+,-./|
00000070  30 31 32 33 34 35 36 37                           |01234567|
00000078
```
On peut observer un décalage des bits du paket entre wireshark et hexdump.


>IFF_NO_The PI tells the kernel that it does not need to provide message    information, that is, it only needs to provide "pure" IP messages and no other bytes.Otherwise (do not set IFF_NO_PI), which adds four extra bytes >>(2-byte identity and 2-byte protocol) at the beginning of the message.
L'option IFF_NO_PI permet donc d'omettre certain bits du message retransmis.

## 3. Un tunnel simple pour IPv6

### 3.1. Redirection du trafic entrant
