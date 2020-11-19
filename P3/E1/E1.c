#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

typedef struct //Definimos la estructura vector info para pasarsela a los hilos ya que solo le podemos pasar un parametro
{
  int *vector;//vector
  int nEle;//Numero de elementos del vector
} vInfo;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Lo utilizo para bloquear el acceso al vector

int num_Ok(int num)//Comprobamos que el numero introducido por el usuario al llamar el programa sea 2 o 5
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

int num_Rand_Camisetas(int nProveedores){return ((rand()%(nProveedores)));}

int num_Rand_Cantidad(){return ((rand()%100)+1);}

void rellenaCamisetas(int *vector, int nEle)
{
	for (int i = 0; i < nEle; ++i)
	{
		vector[i] = num_Rand_Cantidad();//Rellenamos la matriz con numeros aleatorios
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
  stock = (vInfo*) d;
  int camiseta = num_Rand_Camisetas(stock->nEle);
  int cantidad = num_Rand_Cantidad();
  pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
  int diferencia = stock->vector[camiseta]-cantidad;
  if (diferencia < 0)
  {
    printf("(-) El cliente %ld ha comprado %d unidades de la camiseta %d.\n", pthread_self(), stock->vector[camiseta], camiseta+1);
    stock->vector[camiseta] = 0;
    pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
    pthread_exit(NULL);
  }
  stock->vector[camiseta] = diferencia;
  pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
  printf("(-) El cliente %ld ha comprado %d unidades de la camiseta %d.\n", pthread_self(), cantidad, camiseta+1);
  pthread_exit(NULL); 
}

void * th_add (void* d)//Esta será la función que ejecuten las hebras.
{
  vInfo *stock;
  stock = (vInfo*) d;
  int camiseta = num_Rand_Camisetas(stock->nEle);
  int cantidad = num_Rand_Cantidad();
  pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
  stock->vector[camiseta] += cantidad;
  pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
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
    vInfo stock;//Creamos un vector que almacenará el stock de camisetas
    stock.nEle = nProveedores;
    stock.vector = reservarMemoria(stock.nEle);

    srand(time(NULL));//Generamos la semilla para los números aleatorios

    rellenaCamisetas(stock.vector, stock.nEle);
    imprimeCamisetas(stock.vector, stock.nEle);

    for (int i = 0; i < nClientes; i++)//Crearemos tantos hilos como haya indicado el usuario
    {
      //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
    	if(pthread_create(&(threadClientes[i]), NULL, (void*) th_compra, (void*) &stock))
      /*
      El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. 
      Le pasamos como parámetro un puntero a una de las estructuras del vector de estructuras vInfo
      Como los vectores se almacenan seguidos en memoria en cada iteración hacemos que avance el puntero multiplicando el iterador por
      el tamaño de un elemento de la estructura vInfo y sumando dicho valor al puntero vChilds que apunta al primer elemento del vector
      de esta forma en la primera iteración vChilds apuntará al elemento [0] del vector en la segunda al elemento [1] y así sucesivamente
      Funciona de forma similar a como lo haciamos con fichero binarios en los que guardabamos una estructura y teniamos que indicarle al
      puntero el tamaño de las estructuras almacenadas para que en cada iteración saltase uno.
      */
	    {
	      fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	      exit(EXIT_FAILURE); //Finalizamos la ejecución
	    }
    }
    
    for (int i = 0; i < nProveedores; i++)//Crearemos tantos hilos como haya indicado el usuario
    {
      //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
    	if(pthread_create(&(threadProveedores[i]), NULL, (void*) th_add, (void*) &stock))
      /*
      El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. 
      Le pasamos como parámetro un puntero a una de las estructuras del vector de estructuras vInfo
      Como los vectores se almacenan seguidos en memoria en cada iteración hacemos que avance el puntero multiplicando el iterador por
      el tamaño de un elemento de la estructura vInfo y sumando dicho valor al puntero vChilds que apunta al primer elemento del vector
      de esta forma en la primera iteración vChilds apuntará al elemento [0] del vector en la segunda al elemento [1] y así sucesivamente
      Funciona de forma similar a como lo haciamos con fichero binarios en los que guardabamos una estructura y teniamos que indicarle al
      puntero el tamaño de las estructuras almacenadas para que en cada iteración saltase uno.
      */
	    {
	      fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	      exit(EXIT_FAILURE); //Finalizamos la ejecución
	    }
    }
    
    for (int i = 0; i < nClientes; i++)//Con este bucle recogemos a todas las hebras
    {
    	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
    	de un puntero a int. Al pasar la dirección de memoria de un puntero por referencia, el contenido
    	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
    	memoria de un entero que se devuelve en "pthread_exit((void **)suma)". Por tanto, "int * ret" ahora
    	apunta a donde apuntaba "int * suma", solo que está casteado a void.*/
      if(pthread_join(threadClientes[i], NULL)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    } 
    }

    for (int i = 0; i < nProveedores; i++)//Con este bucle recogemos a todas las hebras
    {
    	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
    	de un puntero a int. Al pasar la dirección de memoria de un puntero por referencia, el contenido
    	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
    	memoria de un entero que se devuelve en "pthread_exit((void **)suma)". Por tanto, "int * ret" ahora
    	apunta a donde apuntaba "int * suma", solo que está casteado a void.*/
      if(pthread_join(threadProveedores[i], NULL)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    } 
    }
    
    imprimeCamisetas(stock.vector, stock.nEle);

    exit(EXIT_SUCCESS);
}
