int tun_alloc(char *dev, int flags);
int continuousTransfert(int fromFileDesc, int toFileDesc);
int iftun(char *dev, char *configScript, int toFileDesc);