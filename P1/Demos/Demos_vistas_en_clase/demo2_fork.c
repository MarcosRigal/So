#include <stdio.h>
#include <unistd.h> // fork, pid_t
#include <errno.h> // errno
#include <string.h> // strerror
#include <stdlib.h> // EXIT_SUCCESS

int main () {
    pid_t pid;
    pid = fork();

    switch (pid) {
        case -1:
            printf("Error al hacer fork: %s\n", strerror(errno));
            return EXIT_FAILURE;
        case 0:
            printf("[%d] Soy el proceso hijo\n", getpid());
            //sleep(1);
            printf("[%d] Terminando el proceso hijo\n", getpid());
            break;
        default:
            printf("[%d] Soy el proceso padre\n", getpid());
            //sleep(2);
            printf("[%d] Terminando el proceso padre\n", getpid());
            break;
    }
    return EXIT_SUCCESS;
}
