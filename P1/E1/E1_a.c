#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  int n,status;
  printf("Introduzca el número de procesos hijo que desea generar: ");
  scanf("%d", &n);
  getchar();
  printf("Soy %d el padre de todos\n", getpid());
  for (int i = 0; i < n; i++)
  {
    switch(fork())
    {
      case 0:
        printf("Soy %d el hijo nº %d del proceso: %d\n", getpid(), (i+1), getppid());
        return 0;

      case -1:
        printf("Error al crear el proceso hijo\n");
        return -1;

      default:
        printf("Esperando a que acabe mi hijo nº %d\n", i+1);
        wait(&status);
        printf("Ya ha acabado mi hijo con el codigo de salida: %d\n", WEXITSTATUS(status));
    }
  }
  return 0;
}