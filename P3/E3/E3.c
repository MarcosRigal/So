#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

int *gBuffer;
int nEle = 0;
int pointer = 0;
sem_t fillCount;
sem_t mutex;
sem_t emptyCount;

void imprimeVector ()
{
	printf("[");
  printf(" %d ",gBuffer[0]);//Imprimimos los elementos del vector
	for (int i = 1; i < nEle; ++i)
	{
		printf(", %d ",gBuffer[i]);//Imprimimos los elementos del vector
	}
	printf("]\n");
  printf("\n");
}

void limpiaVector()
{
  for (int i = 0; i < nEle; i++)
  {
    gBuffer[i]=0;
  } 
}

void costumer(void* nIt)
{
  int *nIterations = (int*) nIt;
  while (*nIterations > 0)
  {
    sem_wait(&fillCount);
      sem_wait(&mutex);
        gBuffer[pointer-1] = 0;
        printf("El cliente %ld ha comprado el artículo nº %d | Stock actual: ", pthread_self(), pointer);
        imprimeVector();
        --pointer;
      sem_post(&mutex);
    sem_post(&emptyCount);
    --*nIterations;
  }
  pthread_exit(NULL); 
}

void producer(void* nIt)
{
  int *nIterations = (int*) nIt;
  while (*nIterations > 0)
  {
    sem_wait(&emptyCount);
      sem_wait(&mutex);
        gBuffer[pointer] = 1;
        ++pointer;
        printf("El productor %ld ha añadido el artículo nº %d | Stock actual: ", pthread_self(), pointer);
        imprimeVector();
      sem_post(&mutex);
    sem_post(&fillCount);
    --*nIterations;
  }
  pthread_exit(NULL); 
}

int main(int argc, char const *argv[])
{   
  int select = 1;
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
    if(select==0)
    {
      printf("Hasta la próxima.\n");
      exit(EXIT_SUCCESS);
    }
    else if (select==1)
    {
      int buffer[5];
      gBuffer = buffer;
      nEle = 5;
      limpiaVector();
      int prod_Cons[2];
      pthread_t threads[2];
      sem_init(&mutex, 0, 1);
      sem_init(&fillCount, 0, 0);
      sem_init(&emptyCount, 0, 5);

      for (int i = 0; i < 2; i++)//Crearemos tantos hilos como haya indicado el usuario
      {
        prod_Cons[i]=10;
        //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
      	if (i<1)
        {  
          if(pthread_create(&(threads[i]), NULL, (void*) producer, (void*) prod_Cons+(sizeof(int*)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
        else
        {
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
    else if (select==2)
    {
      int buffer[3];
      gBuffer = buffer;
      nEle = 3;
      limpiaVector();
      int prod_Cons[6];
      pthread_t threads[6];
      sem_init(&mutex, 0, 1);
      sem_init(&fillCount, 0, 0);
      sem_init(&emptyCount, 0, 3);

      for (int i = 0; i < 6; i++)//Crearemos tantos hilos como haya indicado el usuario
      {
        prod_Cons[i]=1;
        //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
      	if (i<3)
        {  
          if(pthread_create(&(threads[i]), NULL, (void*) producer, (void*) prod_Cons+(sizeof(int)*i)))
	        {
	          fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	          exit(EXIT_FAILURE); //Finalizamos la ejecución
	        }
        }
        else
        {
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
      getchar();//Recogemos el enter
    }

  }

}