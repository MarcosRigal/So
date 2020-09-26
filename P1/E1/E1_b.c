#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  int n, child, status;
  printf("Introduzca el número de procesos hijo que desea generar: ");
  scanf("%d", &n);
  printf("Soy %d el padre de todos\n", getpid());
  for (int i = 0; i < n; i++)
  {
    child = fork();
    switch(child)
    {
      case 0:
        printf("Soy %d el hijo del proceso: %d\n", getpid(), getppid());
        break;

      case -1:
        printf("Error al crear el proceso hijo\n");
        return -1;

      default:
        printf("Esperando a que acabe mi hijo %d\n", child);
        wait(&status);
        printf("Ya ha acabado mi hijo %d con el codigo de salida: %d\n", child, WEXITSTATUS(status));
        return 0;
    }
  }
}
