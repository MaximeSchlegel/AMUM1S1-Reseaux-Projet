int tun_alloc(char *dev, int flags);
int continuousReadWrite(int fromFD, int toFD, int verbose);
int iftun(char *dev, char *configScript, int toFD, int verbose);