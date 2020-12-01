 #include "iftun.h"

int main(int argc, char **argv) {
    char script[] = "./configure-tun.sh";
    char *ptr = script;
    iftun(argv[1], ptr, 1);
}