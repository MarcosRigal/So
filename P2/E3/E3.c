#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct //Definimos la estructura vector info para pasarsela a los hilos ya que solo le podemos pasar un parametro
{
  int *vector;//vector
  int nEle;//Numero de elementos del vector
} vInfo;

int num_Ok(int nVec)//Comprobamos que el numero introducido por el usuario al llamar el programa sea 2 o 5
{
  if (nVec==2)
  {
    return 1;//Como es 2 devolvemos 1 para que el programa continue con su ejecución
  }
  if (nVec==5)
  {
    return 1;//Como es 5 devolvemos 1 para que el programa continue con su ejecución
  }
  return 0;//Como no es ni 2 ni 5 devolvemos 0 para que finalice la ejecución del programa
}

int * reservarMemoria(int nEle)//Reservamos memoria para el vector
{
	int *vector;
	vector = (int *)malloc (nEle*sizeof(int));//Reservamos memoria para el vector
	return vector;//Devolvemos el vector
}

void rellenaVector(int *vector, int nEle)
{
  srand(time(NULL));//Generamos la semilla para los números aleatorios
	for (int i = 0; i < nEle; ++i)
	{
		vector[i] = ((rand()%9)+1);//Rellenamos la matriz con numeros aleatorios
	}
}

void imprimeVector (int *vector, int nEle)
{
	printf("[");
  printf(" %d ",vector[0]);//Imprimimos los elementos del vector
	for (int i = 1; i < nEle; ++i)
	{
		printf(", %d ",vector[i]);//Imprimimos los elementos del vector
	}
	printf("]\n");
  printf("\n");
}

vInfo * creaHijos(int* vParent, int nThreads)
{
  int nEle = 10 / nThreads;//Calculamos el numero de elementos del vector hijo
	vInfo * vChilds = (vInfo *)malloc (nThreads*sizeof(vInfo));//Reservamos memoria para el vector de estructuras vInfo
	for (int i=0;i<nThreads;i++)
	{
		vChilds[i].vector = reservarMemoria((nEle));//Reservamos memoria para los vectores que hay dentro de las estructuras vInfo
    vChilds[i].nEle = nEle;//Guardamos en las estructuras el numero de elementos que tiene su vector
    int k = 0;//Definimos un iterador para el bucle que se pone a 0 siempre que pasa por ahí ya que los vectores de dentro de la estructura van siempre de 0 a 1 o de 0 a 4 no como la j que puede ir de 0 a 1, 0 a 4 , 5 a 9, 2 a 3 ...
    for (int j = (i*nEle); j < ((i*nEle)+nEle); j++)
    {
      vChilds[i].vector[k] =vParent[j];//Le asignamos los valores correspondientes del padre
      k++;
    }
    printf("El vector hijo es: \n");
    imprimeVector(vChilds[i].vector, vChilds->nEle);//Imprimimos el vector "Hijo" resultante 
	}
  return vChilds;//Devolvemos el vector de estructuras que contienen a los vectores hijo
}

void * th_sum (void* d)//Esta será la función que ejecuten las hebras.
{
  int *suma = malloc(sizeof(int));//Reservamos memoria para un puntero de tipo int en el monticulo ya que si no desaparece
  *suma = 0;//Como lo vamos a usar de contador lo ponemos a 0
  vInfo *v;//Creamos un puntero para deshacer el casting a void de d ya que apunta a un vector de estructuras de tipo vInfo
  v = (vInfo*) d;
  for (int i = 0; i < v->nEle; i++)
  {
    *suma += v->vector[i];// Sumamos los elementos del vector
  }
	/*
  A continuación estamos devolviendo un puntero a int al que se le hace un casting a void.
	Esto significa que estaremos devolviendo la dirección de memoria de un entero. Este entero
  se ha reservado en el montículo del proceso o hilo principal, que es accesible a todas los hilos creados.
  */		 
  printf("La suma de los elementos de este vector es: %d \n", *suma);//Imprimimos la suma de los elementos de este vector
	pthread_exit((void**) suma);//Devolvemos la suma de los elementos del vector con pthread_exit
}

int main(int argc, char const *argv[])
{
  if (argc < 2 || num_Ok(atoi(argv[1]))==0)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error llame al programa de la siguiente forma:\n");
    printf("./E3 2. o /E3 5\n");
    exit(EXIT_FAILURE);//Como no se ha llamado al programa adecuadamente devolvemos un error
  }
  int nThreads = atoi(argv[1]);//Guardamos el numero de hilos que quiere crear el usuario

  vInfo *vParent = malloc(sizeof(vInfo)); //Reservamos memoria para el vector
  vParent->nEle = 10;//Este es el "vector padre" que siempre va a tener 10 elementos
  
  vParent->vector = reservarMemoria (vParent->nEle);//Reservamos memoria para el vector
  rellenaVector(vParent->vector, vParent->nEle);//Rellenamos el vector con numeros aleatorios del 1 al 9
  printf("El vector padre es: \n");
  imprimeVector(vParent->vector, vParent->nEle);//Imprimimos el vector para comprobar luego el resultado

  vInfo *vChilds = creaHijos(vParent->vector ,nThreads); //Creamos un vector de estructuras vInfo que contendrá los vectores "hijos"

  pthread_t thread[nThreads];//Creamos un vector para almacenar los identificadores de los hilos
  int *ret, suma = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, suma almacenará la suma de los valores devueltos por los hilos
  for (int i = 0; i < nThreads; i++)//Crearemos tantos hilos como haya indicado el usuario
  {
    //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
  	if(pthread_create(&(thread[i]), NULL, (void*) th_sum, (void*) vChilds+(sizeof(vInfo)*i)))
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
  for (int i = 0; i < nThreads; i++)//Con este bucle recogemos a todas las hebras
  {
  	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
  	de un puntero a int. Al pasar la dirección de memoria de un puntero por referencia, el contenido
  	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
  	memoria de un entero que se devuelve en "pthread_exit((void **)suma)". Por tanto, "int * ret" ahora
  	apunta a donde apuntaba "int * suma", solo que está casteado a void.*/
    if(pthread_join(thread[i], (void**) &ret)) 
	  {//Entramos si falla el join
	    fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	    exit(EXIT_FAILURE);//Salimos
	  } 
    suma += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
  }     
  printf("\nLa suma de los numeros de todos los vectores es: %d\n", suma);//Imprimimos la suma
  exit(EXIT_SUCCESS);//Salimos indicando que todo ha ido bien
}