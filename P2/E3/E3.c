#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
El razonamiento para solucionar este ejercicio es el siguiente:
Nos piden un vector de diez elementos dividido en 2 o 5 subvectores lo que equivale a una matriz
2x5 o 5x2 respectivamente es por ello que creamos una de esas dos matrices en base al numero introducido
por la terminal al llamar al programa. Cada hilo recibe un puntero a una estructura que contiene la matriz
su numero de filas y columnas y un contador que se incrementará de 1 en 1 para evitar que se superpongan los 
valores se hace uso de los semaforos pero con el objetivo de que las otras hebras permanezcan bloqueadas el menor
tiempo posible se guarda el valor de i en una variable auxiliar que será la que utilicemos para desbloquear el uso de i a otras hebras
*/

typedef struct //Definimos la estructura matriz info para pasarsela a los hilos ya que solo le podemos pasar un parametro
{
  int **matriz;//Matriz
  int nFil;//Numero de filas de la matriz
  int nCol;//Numero de columnas de la matriz
  int i;//Iterador para acceder a los diferentes vectores de la matriz
} minfo;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Lo utilizo para bloquear el acceso a i 

int num_Ok(int nFil)//Comprobamos que el numero introducido por el usuario al llamar el programa sea 2 o 5
{
  if (nFil==2)
  {
    return 1;//Como es 2 devolvemos 1 para que el programa continue con su ejecución
  }
  if (nFil==5)
  {
    return 1;//Como es 5 devolvemos 1 para que el programa continue con su ejecución
  }
  return 0;//Como no es ni 2 ni 5 devolvemos 0 para que finalice la ejecución del programa
}

int ** reservarMemoria(int nFil, int nCol)//Reservamos memoria para la matriz
{
	int **matriz;
	matriz = (int **)malloc (nFil*sizeof(int *));//Reservamos memoria para el vector de filas
	for (int i=0;i<nFil;i++)
	{
		matriz[i] = (int *) malloc (nCol*sizeof(int));//Reservamos memoria para los vectores de las columnas
	}
	return matriz;//Devolvemos la matriz
}

void rellenaMatriz(int **matriz, int nFil, int nCol)
{
  srand(time(NULL));//Generamos la semilla para los números aleatorios
	for (int i = 0; i < nFil; ++i)
	{
		for (int j = 0; j < nCol; ++j)
		{
			matriz[i][j] = ((rand()%9)+1);//Rellenamos la matriz con numeros aleatorios
		}
	}
}

void imprimeMatriz (int **matriz, int nFil, int nCol)
{
	printf("La matriz es:\n");
	printf("\n");
	for (int i = 0; i < nFil; ++i)
	{
		printf("|");
		for (int j = 0; j < nCol; ++j)
		{
			printf(" %d ",matriz[i][j]);//Imprimimos los elementos de la matriz
		}
		printf("|\n");
	}
}

void * th_sum (void* d)//Esta será la función que ejecuten las hebras.
{
  int *suma = malloc(sizeof(int));//Reservamos memoria para un puntero de tipo int
  *suma = 0;//Como lo vamos a usar de contador lo ponemos a 0
  minfo *m;//Creamos un puntero para deshacer el casting a void de arg ya que argv lo hemos definido como const char
  m = (minfo*) d;
  pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de i
  int n = ++m->i;//Almacenamos el valor de i en n con el objetivo de tener el resto de hebras el menor tiempo posible bloqueadas ya que la otra opción es trabajar con i directamente y mantenerlas bloqueadas mientras se ejecuta el bucle
  pthread_mutex_unlock(&lock);//Como ya hemos acabado con i la desbloqueamos y dejamos que otra hebra la utilice
  for (int j = 0; j < m->nCol; j++)
  {
    *suma += m->matriz[n][j];//Accedemos únicamente a las columnas de la fila i de la matriz lo que equivale a uno de los vectores
  }
	/*
  A continuación estamos devolviendo un puntero a int al que se le hace un casting a void.
	Esto significa que estaremos devolviendo la dirección de memoria de un enter. Este entero
  se ha reservado en el montículo del proceso o hilo principal, que es accesible a todas los hilos creados.
  ¿Que significa la ultima frase?
  */		 
	pthread_exit((void**) suma);//Devolvemos el valor del numero de lineas con pthread_exit
}

int main(int argc, char const *argv[])
{
  minfo *m = malloc(sizeof(minfo));
  m->i=-1;
  m->nFil = atoi(argv[1]);//Guardamos en nFil el número pasado como argumento ya que este será el numero de filas de la matriz de numeros y el numero de hilos que generaremos
  if (argc < 2 || num_Ok(m->nFil)==0)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error llame al programa de la siguiente forma:\n");
    printf("./E3 2. o /E3 5\n");
  }
  
  m->nCol = 10/m->nFil; //Creamos un puntero para la matriz y definimos el número de columnas que será 10/nFil
  m->matriz = reservarMemoria (m->nFil, m->nCol);//Reservamos memoria para la matriz
  rellenaMatriz(m->matriz, m->nFil, m->nCol);//Rellenamos la matriz con numeros aleatorios del 1 al 9
  imprimeMatriz(m->matriz, m->nFil, m->nCol);//Imprimimos la matriz para comprobar luego el resultado

  pthread_t thread[m->nFil];//Creamos un vector para almacenar los identificadores de los hilos
	int *ret, suma = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, suma almacenará la suma de los valores devueltos por los hilos
  for (int i = 0; i < m->nFil; i++)//Crearemos tantos hilos como filas tenga la matriz
  {
    //En este bucle es donde creamos los nFil hilos que ejecutarán la función th_rand
  	if(pthread_create(&(thread[i]), NULL, (void*) th_sum, (void*) m))
    /*
    El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. 
    Le pasamos como parámetro la estructura de tipo minfo
    */
	  {
	    fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	    exit(EXIT_FAILURE); //Finalizamos la ejecución
	  }
  }
  for (int i = 0; i < m->nFil; i++)//Con este bucle recogemos a todas las hebras
  {
  	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
  	de un puntero a float. Al pasar la dirección de memoria de un puntero por referencia, el contenido
  	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
  	memoria de un flotante que se devuelve en "pthread_exit((void *)z)". Por tanto, "float * ret" ahora
  	apunta a donde apuntaba "float * z", solo que está casteado a void.*/
    if(pthread_join(thread[i], (void**) &ret)) 
	  {//Entramos si falla el join
	    fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	    exit(EXIT_FAILURE);//Salimos
	  } 
    suma += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
  }     
  printf("La suma de los numeros de todos los vectores es: %d\n", suma);//Imprimimos la suma
  exit(EXIT_SUCCESS);//Salimos indicando que todo ha ido bien
}