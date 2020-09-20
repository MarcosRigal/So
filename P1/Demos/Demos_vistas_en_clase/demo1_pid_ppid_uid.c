#include <stdio.h>
#include <unistd.h> // getpid, getppid, getuid
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    pid_t my_pid, my_parent_pid;
    uid_t my_uid;

    my_pid = getpid();
    my_parent_pid = getppid();
    my_uid = getuid();

    printf("Mi PID es %d\n", my_pid);
    printf("El PID de mi padre es %d\n", my_parent_pid);
    printf("Mi ID de usuario es %d\n", my_uid);
    
    //sleep(60);

    return EXIT_SUCCESS;
}
