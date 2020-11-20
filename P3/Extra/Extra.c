/*
La idea tras la implementación es la siguiente: 

Se quiere comprobar que pasa si una hebra que ha ejecutado un pause dentro de un 
pthread_mutex_lock(&lock) recibe una SIGKILL sin haber ejecutado pthread_mutex_unlock(&lock).
¿Que pasa con la otra hebra se queda bloqueada para siempre?
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Lo utilizo para bloquear el acceso a la sección critica


int global=0;//Declaramos una variable globala

void * h1()//Esta será la función que ejecuten las hebra 1
{
  pthread_mutex_lock(&lock);//Bloqueamos el acceso a la sección critica
  pause();//La ponemos a esperar a SIGKILL
  printf("Hola desde la hebra 1\n");//Esto no se debe ejecutar 
  global++;//Incrementa la variable global
  pthread_mutex_unlock(&lock);//Desbloqueamos el acceso a la sección critica
  pthread_exit(NULL);//Una vez la hebra ha acabado finaliza su ejecución
}

void * h2()//Esta será la función que ejecuten la hebra 2
{  
  pthread_mutex_lock(&lock);//Bloqueamos el acceso a la sección critica
  printf("Hola desde la hebra 2\n");
  global++;//Incrementará la variable global
  pthread_mutex_unlock(&lock);//Desbloqueamos el acceso a la sección critica
  pthread_exit(NULL);//Una vez la hebra ha acabado finaliza su ejecución
}

int main()
{
  pthread_t hebra1, hebra2; //Almacena el identificador cada una de las hebras
  printf("Soy la hebra principal, el valor de variable global es: %d\n", global);
	if(pthread_create(&hebra1, NULL, (void *) h1, NULL))//El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. El segundo null es un puntero para los argumentos que le queramos pasar a la función que va a ejecutar la hebra
	{
	  fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	  exit(EXIT_FAILURE); 
	}
	if(pthread_create(&hebra2, NULL, (void *) h2, NULL))
	{
	  fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	  exit(EXIT_FAILURE); 
	}
   printf("Soy la hebra principal, el valor de variable global es: %d\n", global);//Vemos si en el escenario 1 la hebra 2 ha tenido tiempo de incrementar el valor de la variable global(no suele verse reflejado por como funciona printf)
   pthread_kill(hebra1, SIGKILL);//Mandamos la señal
  //Espera de la hebra1
	if(pthread_join(hebra1, NULL)) 
	{//Entramos si falla el join
	  fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	  exit(EXIT_FAILURE);//Salimos
	}
  //Espera de la hebra2
	if(pthread_join(hebra2, NULL)) 
	{//Entramos si falla el join
	  fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	  exit(EXIT_FAILURE);//Salimos
	}  
  printf("El valor de la variable global es: %d\n", global);//Imprimimos el valor de la variable global
  exit(EXIT_SUCCESS);
}