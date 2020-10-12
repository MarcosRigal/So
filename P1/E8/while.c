#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void printid()
{//Macro que ejecutará el proceso al recibir la señal
  printf("Mi id es: %d\n", getpid());
}

int main()
{
  printf("Mi id es: %d\n", getpid());//El proceso se identifica
  signal(SIGUSR1, printid);//Definimos la reacción del proceso a la señal SIGUSR1
  while (1)
  {
    pause();//Dejamos el proceso a la espera de señales
  }
  exit(EXIT_SUCCESS);//Aqui no deberiamos llegar pero por si acaso
}
