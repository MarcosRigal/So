#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char const *argv[])
{
  int status; //Variable que almacenará el retorno de kill
  if (argc < 2)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error falta el id del programa al que se le va a enviar la señal.");
    exit(EXIT_FAILURE);
  }
  printf("PID del proceso al que le enviaremos la señal: %d\n", atoi(argv[1])); //Imprimimos el id del proceso al que le enviamos la señal
  status = kill(atoi(argv[1]), SIGUSR1);//Le enviamos la señal y almacenamos lo que nos devuelve kill
  if (!status)//Comprobamos que la señal se haya enviado correctamente
  {
   printf("La señal se envió satisfactoriamente.\n");  
  }
  else
  {
    printf("Error al enviar la señal. Valor de erno: %d, definido como: %s\n", errno, strerror(errno));
  }
  sleep(1);
  status = kill(atoi(argv[1]), SIGKILL);//Le enviamos la señal y almacenamos lo que nos devuelve kill
  if (!status)//Comprobamos que la señal se haya enviado correctamente
  {
   printf("La señal se envió satisfactoriamente.\n");  
  }
  else
  {
    printf("Error al enviar la señal. Valor de erno: %d, definido como: %s\n", errno, strerror(errno));
  }
  exit(EXIT_SUCCESS);//Como todo ha ido bien el proceso padre acaba exitosamente
}