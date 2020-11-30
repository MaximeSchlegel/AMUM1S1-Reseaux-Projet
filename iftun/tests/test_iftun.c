 #include "iftun.h"

int main(int argc, char **argv) {
    iftun(argv[1], "./configure-tun.sh", 1);
}