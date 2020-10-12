#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
int i=0;

void RING()
{//Este es el método que ejecutará el programa cada vez que reciba una alarma
  printf("RING\n");
  i++;//Incrementamos la variable global
  if (i==4)//Entrará si la variable global vale 4 es decir si han sonado más de 4 alarmas
  {
    kill(getpid(), SIGKILL);//Le envia una señal al proceso para que acane
  }
  
}

int main(int argc, char const *argv[])
{
  signal(SIGALRM, RING);//Aquí le indicamos al proceso como reaccionar cuando reciba una señal de tipi SIGALARM (una alarma)
  alarm(5);//Programamos una alarma para dentro de 5 segundos
  pause();//Ponemos el proceso en pausa hasta que suene la alarma
  alarm(3);//Programamos una alarma para dentro de 3 segundos
  pause();//Ponemos el proceso en pausa hasta que suene la alarma
  while (1)
  {//Este es un bucle infinito del que el programa no va a salir
    alarm(1);//Programamos una alarma para dentro de 1 segundo
    pause();//Ponemos el proceso en pausa hasta que suene la alarma
  } 
  exit(EXIT_SUCCESS);//En teoría aquí no deberíamos llegamar nunca pero por si acaso
}
