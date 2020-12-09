# Projet Réseau

## Setup

La compilation des sources se fait avec le make fil ci-joint.
Le goal all doit compiler les sources et placé tous les fichier necessaire au set up du tunnel sur les VM dans "/partage/tunnel/"

L'infrasture complete peut se lancer simplement (sous windows) avec un appel à up.cmd (reciproquement pu down.cmd).

## Utilisation

Les excutables renvoie leur arguments lorsque que leur appel n'est pas correct.

Pour créer le tunnel sur la VMX:
```
> sudo -s
> cd /vagrant/
> salt-call state.apply
> cd /mnt/partage/tunnel
> ./tunnel64d [-v] ./tun-VMX.conf
```
il est possible d'ajouter l'option [-rad X] mais la publicité ne marche pas. 

Lorsque VM2-6 est down, il faut appliquer la configuration ./configure-vmX.sh aux VM1-6 et VM3-6 pour mofifier leur route

## Attention

Il se peut que des erreur apparaissent lors de l'execution des scripts de configuration. Dans ce cas il faut vérifié leir sauf de ligne (et les convertir en LF si nessecaire).