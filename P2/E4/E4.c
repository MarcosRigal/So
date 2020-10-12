#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int global=0;//Declaramos una variable global

void * incrementa()//Esta será la función que ejecuten las hebras cuando las llamemos
{
  for (int i = 0; i < 10000; i++)
  {
    global++;//Cada hebra incrementará 1000 veces la variable global
  }
  pthread_exit(NULL);//Una vez la hebra ha acabado finaliza su ejecución
}

int main()
{
  pthread_t hebra1, hebra2; //Almacena el identificador cada una de las hebras
  printf("Soy la hebra principal, el valor de variable global es: %d\n", global);
	if(pthread_create(&hebra1, NULL, (void *) incrementa, NULL))//El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. El segundo null es un puntero para los argumentos que le queramos pasar a la función que va a ejecutar la hebra
	{
	  fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crar la hebra
	  exit(EXIT_FAILURE); 
	}
	if(pthread_create(&hebra2, NULL, (void *) incrementa, NULL))
	{
	  fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crar la hebra
	  exit(EXIT_FAILURE); 
	}
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
