#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  int status;
  printf("Soy %d el proceso padre\n", getpid());
  if (fork()==0)
  {
    printf("Soy %d el hijo de %d\n", getpid(), getppid());
    return 0;
  }
  printf("Me voy a echar una siesta mientras espero a mi hijo\n");
  sleep(20);
  wait(&status);
  printf("Ya ha acabado mi hijo con el codigo de salida: %d\n", WEXITSTATUS(status));
  sleep(20);
  return 0;
}
