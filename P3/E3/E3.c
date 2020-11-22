#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int *gBuffer;//Puntero para acceder al buffer de forma global que apunta a un vector cuya longitud se define en tiempo de ejecución
int bufferSize = 0;//Numero de elementos maximos que puede haber en el buffer
int pointerCostumer = 0;//Puntero para la posición del consumidor 
int pointerProducer = 0;//Puntero para la posición del productor
sem_t stock;//Este semaforo se inicializa a 0 lo utilizaremos para saber cuantos elementos hay actualmente en el buffer para ver si el cliente puede coger algo o no
sem_t mutex;//Semaforo que bloquea el acceso a la sección crítica
sem_t huecos;//Este semaforo se inicializa con el tamaño del buffer y lo utilizaremos para saber cuantos elementos más podemos añadir al buffer

void imprimeVector ()
{
	printf("[");
  printf(" %d ",gBuffer[0]);//Imprimimos los elementos del vector
	for (int i = 1; i < bufferSize; ++i)
	{
		printf(", %d ",gBuffer[i]);//Imprimimos los elementos del vector
	}
	printf("]\n");
  printf("\n");
}

void limpiaVector()
{
  for (int i = 0; i < bufferSize; i++)//Pone a 0 todos los elementos del vector
  {
    gBuffer[i]=0;
  } 
}

void costumer(void* nIt)//Funcion que ejecutan los hilos clientes
{
  int *nIterations = (int*) nIt;//Deshacemos el casting
  while (*nIterations > 0)
  {//Este bucle se ejecutará mientras haya iteraciones restantes
    sem_wait(&stock);//Decrementamos el numero de artículos disponibles si el resultado es -0 se bloquea hasta que haya nuevos artículos disponibles
      sem_wait(&mutex);//Bloqueamos el acceso a la sección crítica
        gBuffer[pointerCostumer] = 0;//Para simular que consume ponemos a 0 esa posición del buffer
        printf("El cliente %ld ha comprado el artículo nº %d | Stock actual: ", pthread_self(), pointerCostumer+1);
        imprimeVector();//Imprimimos por pantalla la operación que acabamos de realizar
        pointerCostumer=((pointerCostumer+1)%bufferSize);//Actualizamos el valor del puntero de clientes
      sem_post(&mutex);//Desbloqueamos el acceso a la sección crítica
    sem_post(&huecos);//Aumentamos el número de huecos disponibles para ser rellenados por un productor
    --*nIterations;//Consumimos una iteración
  }
  pthread_exit(NULL); //Salimos
}

void producer(void* nIt)
{
  int *nIterations = (int*) nIt;
  while (*nIterations > 0)//Deshacemos el casting a void
  {
    sem_wait(&huecos);//Decrementamos el numero de huecos si el resultado es menor que 0 espera hasta que un consumidor consuma un elemento
      sem_wait(&mutex);//Bloqueamos el acceso a la sección crítica
        gBuffer[pointerProducer] = 1;//Para simular que añadimos un elemento ponemos a uno esa posición del buffer
        printf("El productor %ld ha añadido el artículo nº %d | Stock actual: ", pthread_self(), pointerProducer+1);
        imprimeVector();//Imprimimos por pantalla el estado del buffer
        pointerProducer= ((pointerProducer+1)%bufferSize);//Actualizamos el valor del puntero de productores
      sem_post(&mutex);//Desbloqueamos el acceso a la sección critica
    sem_post(&stock);//Aumentamos el número de productos disponibles para que los compradores puedan coger
    --*nIterations;//Le restamos una iteración a esta hebra
  }
  pthread_exit(NULL);//Salimos de la hebra
}

int main(int argc, char const *argv[])
{   
  int select = 1;//Variable para el menú de selección
  while (select !=0 )
  {
    system("clear");
    printf(" ______________________________________\n");
    printf("|                                      |\n");
    printf("|--------- Menu de selección: ---------|\n");
    printf("|______________________________________|\n");
    printf("|                                      |\n");
    printf("|Pulsa 1 para ejecutar el apartado A.  |\n");
    printf("|Pulsa 2 para ejecutar el apartado B.  |\n");
    printf("|Pulsa 0 para salir del programa.      |\n");
    printf("|______________________________________|\n");
    printf("\nSu elección: ");
    scanf("%d", &select);
    getchar();
    printf("\n");
    if(select==0)//Finaliza la ejecución
    {
      printf("Hasta la próxima.\n");
      exit(EXIT_SUCCESS);
    }
    else if (select==1)//Apartado a
    {
      int buffer[5];//Creamos el vector buffer de 5 elementos 
      gBuffer = buffer;//Hacemos que el vector global buffer apunte a este vector
      bufferSize = 5;//Indicamos el numero de elementos que tendrá el vector
      limpiaVector();//Lo ponemos a 0
      int prod_Cons[2];//Vector para pasarle las iteraciones a la hebra
      pthread_t threads[2];//Vector para los identificadores de las hebras
      sem_init(&mutex, 0, 1);//Inicializamos el semaforo que controla el acceso a la zona crítica
      sem_init(&stock, 0, 0);//Inicializamos el semaforo que controla el stock de productos
      sem_init(&huecos, 0, 5);//Inicializamos el semaforo que controla el numero de huecos

      for (int i = 0; i < 2; i++)//Crearemos tantos hilos como haya indicado el usuario
      {
        prod_Cons[i]=10;
        //En este bucle es donde creamos los nThreads hilos 
      	if (i<1)
        {//Creamos la hebra productora y le pasamos el numero de iteracioness
          if(pthread_create(&(threads[i]), NULL, (void*) producer, (void*) prod_Cons+(sizeof(int*)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
        else
        {//Creamos la hebra consumidora y le pasamos el numero de iteraciones 
          if(pthread_create(&(threads[i]), NULL, (void*) costumer, (void*) prod_Cons+(sizeof(int)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
      }

      for (int i = 0; i < 2; i++)//Con este bucle recogemos a todas las hebras
      {
        if(pthread_join(threads[i], NULL)) 
	      {//Entramos si falla el join
	        fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	        exit(EXIT_FAILURE);//Salimos
	      }
      }
      exit(EXIT_SUCCESS);
    }
    else if (select==2)//Apartado B
    {
      int buffer[3];//Inicializamos el vector buffer
      gBuffer = buffer;//Apuntamos hacia el el puntero global
      bufferSize = 3;//Almacenamos el tamaño del buffer
      limpiaVector();//Ponemos a 0 todos los elementos del vector
      int prod_Cons[6];//Vector que almacena las iteraciones de los hilos
      pthread_t threads[6];//Vector que almacena los identificadores de los hilos
      sem_init(&mutex, 0, 1);//Inicializamos el semaforo que controla el acceso a la zona crítica
      sem_init(&stock, 0, 0);//Inicializamos el semaforo que controla el stock de productos
      sem_init(&huecos, 0, 3);//Inicializamos el semaforo que controla el numero de huecos

      for (int i = 0; i < 6; i++)//Crearemos tantos hilos como haya indicado el usuario
      {
        prod_Cons[i]=1;
        //En este bucle es donde creamos los nThreads hilos
      	if (i<3)
        {  //Creamos a los productores y les pasamos su numero de iteraciones
          if(pthread_create(&(threads[i]), NULL, (void*) producer, (void*) prod_Cons+(sizeof(int)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
        else
        {//Creamos los consumidores y les pasamos su numero de iteraciones
          if(pthread_create(&(threads[i]), NULL, (void*) costumer, (void*) prod_Cons+(sizeof(int)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
      }
      for (int i = 0; i < 6; i++)//Con este bucle recogemos a todas las hebras
      {  
        if(pthread_join(threads[i], NULL)) 
	      {//Entramos si falla el join
	        fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	        exit(EXIT_FAILURE);//Salimos
	      }
      } 
      exit(EXIT_SUCCESS);
    }

    else//Es el default si se introduce un número que no equivale a ninguna opción
    {
      system("clear");
      printf("Error seleccione una opción valida.\n");
      printf("Pulse enter para volver al menu principal...\n");
      getchar();//Recogemos el enter y volvemos al menú
    }

  }

}