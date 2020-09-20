#include <stdio.h>
#include <unistd.h> // fork, pid_t, getpid
#include <errno.h> // errno
#include <string.h> // strerror
#include <stdlib.h> // EXIT_SUCCESS
#include <sys/wait.h> // wait

int main () {
    pid_t pid;

    printf("Soy el proceso padre con PID %d\n", getpid());
    for (int i=0; i < 5; ++i) {
        pid = fork();

        if (pid == -1) {
            printf("Error al hacer fork: %s\n", strerror(errno));
            break;
        }
        else if (pid == 0) {
            printf("Soy el proceso hijo con PID %d y mi padre %d\n", getpid(), getppid());
            // sleep(2);
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Proceso padre: generado hijo con PID %d\n", pid);
            // sleep(1);
        }
    }

    pid_t hijo_esperado;
    int status;

    while((hijo_esperado = wait(&status)) >= 0) {
        if (WIFEXITED(status)) {
            int codigo_salida_hijo = WEXITSTATUS(status);
            printf("El hijo con PID %d ha terminado normalmente con código %d\n",
                   hijo_esperado, codigo_salida_hijo);
        }

        if (WIFSIGNALED(status)) {
            int senal_terminacion = WTERMSIG(status);
            printf("El hijo con PID %d ha terminado debido a la senal %s",
                   hijo_esperado, strsignal(senal_terminacion));
        }

        // Esto no ocurrirá nunca.
        // Para captar procesos que han sido detenidos, es necesario
        // usar waitpid (en lugar de wait) con ciertos parámetros.
        if (WIFSTOPPED(status)) {
            int senal_terminacion = WSTOPSIG(status);
            printf("El hijo con PID %d fue detenido debido a la senal %s",
                   hijo_esperado, strsignal(senal_terminacion));
        }
    }

    if (errno == ECHILD) {
        printf("Todos los hijos han terminado\n");
    }
    else {
        printf("Error al sincronizar procesos: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
