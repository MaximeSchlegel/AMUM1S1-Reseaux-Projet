int tun_alloc(char *dev, int flags);
int continuousTransfert(int fromFD, int toFD);
int iftun(char *dev, char *configScript, int toFD);