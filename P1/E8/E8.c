#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
  int status;
  if (argc < 2)
  {
    printf("Error falta el id del programa al que se le va a enviar la señal.");
    return -1;
  }
  printf("PID: %d\n", atoi(argv[1]));
  status = kill(atoi(argv[1]), SIGUSR1);
  printf("Kill return: %d\n", status);
  return 0;
}
//no se porque no lo borra
