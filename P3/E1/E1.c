#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

typedef struct //Definimos la estructura vector info para pasarsela a los hilos ya que solo le podemos pasar un parametro
{
  int *vector;//vector
  int nEle;//Numero de elementos del vector
} vInfo;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Semaforo para bloquear el acceso al vector

int num_Ok(int num)//Comprobamos que el numero introducido por el usuario sea mallor que 1
{
  if (num>0)
  {
    return 1;//Como es > que 0 devolvemos 1 para que el programa continue con su ejecución
  }

  return 0;//Como no es > que 0  devolvemos 0 para que finalice la ejecución del programa
}

int * reservarMemoria(int nEle)//Reservamos memoria para el vector
{
	int *vector;
	vector = (int *)malloc (nEle*sizeof(int));//Reservamos memoria para el vector
	return vector;//Devolvemos el vector
}

int num_Rand_Camisetas(int nProveedores){return ((rand()%(nProveedores)));}//Numero de camiseta con el que se va a interactuar hacemos el modulo con el numero de proveedores puesto que si hay n proveedores hay n modelos de camisetas

int num_Rand_Cantidad(){return ((rand()%100)+1);}//Numero de camisetas que vamos a comprar

void rellenaCamisetas(int *vector, int nEle)
{
	for (int i = 0; i < nEle; ++i)
	{
		vector[i] = num_Rand_Cantidad();//Rellenamos el vector de camisetas con numeros aleatorios
	}
}

void imprimeCamisetas(int *vector, int nEle)
{
  printf("\n---------------  Stock de la tienda ---------------\n");
	printf("\n");
  printf(" - Camiseta 1: %d",vector[0]);//Imprimimos los elementos del vector
	for (int i = 1; i < nEle; ++i)
	{
		printf("\n - Camiseta %d: %d", i+1,vector[i]);//Imprimimos los elementos del vector
	}
  printf("\n");
	printf("---------------------------------------------------\n\n");
}

void * th_compra (void* d)//Esta será la función que ejecuten las hebras.
{
  vInfo *stock;
  stock = (vInfo*) d;//Deshacemos el casting a void
  int camiseta = num_Rand_Camisetas(stock->nEle);//Numero de camiseta con la que va a interactuar
  int cantidad = num_Rand_Cantidad();//Cantidad de camisetas que va a comprar
  pthread_mutex_lock(&lock);//Semaforo para bloquear el acceso a la sección crítica
  int diferencia = stock->vector[camiseta]-cantidad;//Calculo el stock tras la compra
  if (diferencia < 0)//Si el stock se queda negativo como el minimo es 0 lo ponemos a 0 y salimos
  {//Imprimo antes de actualizar el valor ya que el cliente compra todo el stock disponible
    printf("(-) El cliente %ld ha comprado %d unidades de la camiseta %d.\n", pthread_self(), stock->vector[camiseta], camiseta+1);
    stock->vector[camiseta] = 0;
    pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
    pthread_exit(NULL);
  }
  stock->vector[camiseta] = diferencia;//Como el stock se quedaría positivo o = a 0 lo asignamos y salimos
  pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
  printf("(-) El cliente %ld ha comprado %d unidades de la camiseta %d.\n", pthread_self(), cantidad, camiseta+1);
  pthread_exit(NULL); 
}

void * th_add (void* d)//Esta será la función que ejecuten las hebras.
{
  vInfo *stock;
  stock = (vInfo*) d;//Deshacemos el casting
  int camiseta = num_Rand_Camisetas(stock->nEle);//Numero de la camiseta con la que vamos a interactuar
  int cantidad = num_Rand_Cantidad();//Numero de unidades de la camiseta que vamos a añadit
  pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
  stock->vector[camiseta] += cantidad;//Actualizamos el valor del vector
  pthread_mutex_unlock(&lock);//Como ya hemos acabado con la sección crítica la desbloqueamos y dejamos que otra hebra la utilice
  printf("(+) El proveedor %ld ha añadido %d unidades de la camiseta %d.\n", pthread_self(), cantidad, camiseta+1);
  pthread_exit(NULL); 
}

int main(int argc, char const *argv[])
{
    if (argc < 3 || num_Ok(atoi(argv[1]))==0 || num_Ok(atoi(argv[2]))==0)//Comprobamos que se haya llamado al programa de forma adecuada
    {
      printf("Error llame al programa de la siguiente forma:\n");
      printf("./E1 (numero de clientes) (numero de proveedores)\n");
      exit(EXIT_FAILURE);//Como no se ha llamado al programa adecuadamente devolvemos un error
    }
    int nClientes = atoi(argv[1]);//Guardamos el numero de clientes
    int nProveedores = atoi(argv[2]);//Guardamos el numero de proveedores
    pthread_t threadClientes[nClientes];//Creamos un vector para almacenar los identificadores de los hilos de los clientes
    pthread_t threadProveedores[nProveedores];//Creamos un vector para almacenar los identificadores de los hilos de los proveedores
    vInfo stock;//Creamos una estructura que almacenará el stock de camisetas
    stock.nEle = nProveedores;//Almacenamos el numero de proveedores ya que es el mismo que el numero de camisetas
    stock.vector = reservarMemoria(stock.nEle);//Reservamos memoria para el vector de camisetas

    srand(time(NULL));//Generamos la semilla para los números aleatorios

    rellenaCamisetas(stock.vector, stock.nEle);//Rellenamos el vector de camisetas
    imprimeCamisetas(stock.vector, stock.nEle);//Imprimimos el vector de camisetas

    for (int i = 0; i < nClientes; i++)//Crearemos tantos hilos como clientes haya
    {
      //En este bucle es donde creamos los hilos clientes que ejecutarán la función th_compra
    	if(pthread_create(&(threadClientes[i]), NULL, (void*) th_compra, (void*) &stock))
	    {
	      fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	      exit(EXIT_FAILURE); //Finalizamos la ejecución
	    }
    }
    
    for (int i = 0; i < nProveedores; i++)//Crearemos tantos hilos como proveedores haya
    {
      //En este bucle es donde creamos los hilos que ejecutarán la función th_add
    	if(pthread_create(&(threadProveedores[i]), NULL, (void*) th_add, (void*) &stock))
	    {
	      fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	      exit(EXIT_FAILURE); //Finalizamos la ejecución
	    }
    }
    
    for (int i = 0; i < nClientes; i++)//Con este bucle recogemos a todas las hebras de clientes
    {
      if(pthread_join(threadClientes[i], NULL)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    } 
    }

    for (int i = 0; i < nProveedores; i++)//Con este bucle recogemos a todas las hebras de proveedores
    {
      if(pthread_join(threadProveedores[i], NULL)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    } 
    }
    
    imprimeCamisetas(stock.vector, stock.nEle);//Imprimimos el estado final del vector de camisetas

    exit(EXIT_SUCCESS);
}
