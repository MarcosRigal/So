#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

void tratarSennal()
{
  printf("Dime papá\n");
}

int main()
{
  int id, status;
  printf("Soy %d el padre\n", getpid());
  id = fork();
  switch(id)
  {
    case 0:
      signal(SIGUSR1, tratarSennal); //¿Con ponerlo una vez ya vale para todo el programa?
      printf("Soy %d el hijo del proceso: %d\n", getpid(), getppid());
      for (int i = 0; i < 5; i++)
      {
        pause();
      }
      return 0;
    
    case -1:
      printf("Error al crear el proceso hijo\n");
      return -1;
    
    default:
      printf("Esperando a que acabe mi hijo\n");
      for (int i = 0; i < 5; i++)
      {
        sleep(3);//Si no uso sleep acaba al momento
        kill(id, SIGUSR1);
      }
      sleep(3);
      kill(id, SIGKILL);
      wait(&status);
      printf("Ya ha acabado mi hijo con el codigo de salida: %d\n", WEXITSTATUS(status));
  }
  return 0;
}