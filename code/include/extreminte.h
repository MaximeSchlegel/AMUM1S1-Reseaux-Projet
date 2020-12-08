int continuousRecvWrite (int fromFD, int toFD, int verbose);
int ext_out(char* localPort, int outputFD, int verbose);
int ext_in(int inputFD, char* remoteIP, char* remotePort, int verbose);
int  ext_bid(char* localPort, int outputFD, int inputFD, char* remoteIP, char* remotePort, int verbose);