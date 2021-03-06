#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <pthread.h>

int main(int argc, const char *argv[])
{
  if (argc<2)
  {//Comprueba que el numero de parámetos introducido sea el correcto
    printf("Error, debe pasar la clave para acceder a la zona de memoria");
  }//Llama a la función factorial e imprime el resultado
 pthread_mutex_t *lock = NULL;//Semaforo para bloquear el acceso al contador

  int id_Memoria = atoi(argv[1]);
  int id_Semaforo = atoi(argv[2]);
  int *contador = NULL;//Puntero a la zona de memoria compartida
  int value;//Almacena el valor devuelto por shmdt al realizar la desconexión
	

  contador = (int *)shmat(id_Memoria, NULL, 0);//Enganchamos el segmento con el proceso
	if (contador == NULL)
	{
		printf("Main() de demo1... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
  
  lock = (pthread_mutex_t *)shmat(id_Semaforo, NULL, 0);//Enganchamos el segmento con el proceso
	if (lock == NULL)
	{
		printf("Main() de demo1... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
  for (int i = 0; i < 100000; i++)
  {//No hace falta linkearlo ya que con el fork hereda el puntero contador del padre
		printf("%d\n",pthread_mutex_lock(lock));
		printf("Soy: %d \n", getpid()); //El hijo se identifica 
		++*contador;
		printf("El valor de la variable es: %d\n", *contador);//Imprime e incrementa el valor de la variable de la zona compartida
		printf("%d\n",pthread_mutex_unlock(lock));
	}

  value = shmdt ((char *)contador);
	if (value == -1)//Comprobamos el valor devuelto por shmdt al realizar la desconexión
	{
	  printf("Error en shmdt...\n");
	  printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
	}
	value = shmdt ((pthread_mutex_t *)lock);
	if (value == -1)//Comprobamos el valor devuelto por shmdt al realizar la desconexión
	{
		printf("Error en shmdt...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);//Como todo ha ido bien el proceso acaba exitosamente
}
