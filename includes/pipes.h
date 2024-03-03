#ifndef pipes
#define pipes

#include <sys/time.h>

typedef struct informacao_processo {
    int pid;
    char nome[50];
    struct timeval time;
    long ms;
} informacao_processo;

#endif