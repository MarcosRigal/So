#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int impar = 0;
int par = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Lo utilizo para bloquear el acceso al vector

int num_Ok(int num)//Comprobamos que el numero introducido por el usuario al llamar el programa sea 2 o 5
{
  if (num>0)
  {
    return 1;//Como es > que 0 devolvemos 1 para que el programa continue con su ejecución
  }

  return 0;//Como no es > que 0  devolvemos 0 para que finalice la ejecución del programa
}

int num_Rand(){return ((rand()%11));}

int th_sum(void* orden)
{
  int *n = (int*) orden;
  int *suma = malloc(sizeof(int));//Reservamos memoria para un puntero de tipo int en el monticulo ya que si no desaparece
  *suma = 0;//Como lo vamos a usar de contador lo ponemos a 0
  int rand;
  if (*n%2==0)
  {
    for (size_t i = 0; i < 5; i++)
    {
      rand = num_Rand();
      *suma += rand;
      pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
      par += rand;
      pthread_mutex_unlock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
    }
    pthread_exit((void*) suma);//Devolvemos la suma de los elementos del vector con pthread_exit
  }
  for (size_t i = 0; i < 5; i++)
  {
    rand = num_Rand();
    *suma += rand;
    pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
    impar += rand;
    pthread_mutex_unlock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
  }
  pthread_exit((void*) suma);//Devolvemos la suma de los elementos del vector con pthread_exit
}

int main(int argc, char const *argv[])
{
  if (argc < 2 || num_Ok(atoi(argv[1]))==0)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error llame al programa de la siguiente forma:\n");
    printf("./E1 (numero de hebras)\n");
    exit(EXIT_FAILURE);//Como no se ha llamado al programa adecuadamente devolvemos un error
  }
  int nThreads = atoi(argv[1]);//Guardamos el numero de clientes
  int *ret, sumapar = 0, sumaimpar = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, suma almacenará la suma de los valores devueltos por los hilos
  pthread_t threads[nThreads];//Creamos un vector para almacenar los identificadores de los hilos de los clientes
  srand(time(NULL));//Generamos la semilla para los números aleatorios
  int contador[nThreads];

  for (int i = 0; i < nThreads; i++)//Crearemos tantos hilos como haya indicado el usuario
  {
    contador[i]=i+1;
    //En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
  	if(pthread_create(&(threads[i]), NULL, (void*) th_sum, (void*) contador+(sizeof(int)*i)))
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
    if ((i+1)%2==0)
    {      
      if(pthread_join(threads[i], (void*) &ret)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    }
      printf("El valor devuelto por la hebra nº %d es: %d\n", i+1, *ret); 
      sumapar += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
    }
    else
    {
      if(pthread_join(threads[i], (void*) &ret)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    }
      printf("El valor devuelto por la hebra nº %d es: %d\n", i+1, *ret); 
      sumaimpar += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
    }
  }     
  printf("\nValor de la variable compartida impar: %d\n", impar);
  printf("La suma de los valores devueltos por las hebras impares es: %d\n", sumaimpar);
  
  printf("\nValor de la variable compartida par: %d\n", par);
  printf("La suma de los valores devueltos por las hebras pares es: %d\n", sumapar);
  exit(EXIT_SUCCESS);
}
