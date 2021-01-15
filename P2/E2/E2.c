#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void * th_file (void * argv)//Esta será la función que ejecuten las hebras.
{
  int *nlines = malloc(sizeof(int));//Reservamos memoria para un puntero de tipo int
  *nlines = 0;//Como lo vamos a usar de contador lo ponemos a 0
  char const *fname;//Creamos un puntero para deshacer el casting a void de arg ya que argv lo hemos definido como const char
  char aux[256];//Creamos una cadena auxiliar para hacer los fgets
  fname = (char*) argv;//Deshacemos el casting
  FILE* f = fopen(fname, "r"); //Creamos un puntero para abrir el fichero que cuyo nombre se encuentra en fname y con fopen("r") lo abrimos en modo lectura
  if (f==NULL) //Comprobamos que el fichero se haya abierto correctamente
  {
    printf("Error al abrir el archivo\n"); //Como no se ha abierto correctamente informamos al usuario de que ha habido un error
    EXIT_FAILURE; //Salimos y devolviendo una señal de error
  }
  while(fgets(aux, 256, f)!=NULL)//fgets nos devuelve null si ya no hay más lineas para leer
  {
    ++*nlines;//Como hemos podido leer una linea sumamos incrementamos el contador del numero de lineas
  }
  fclose(f);//Como ya hemos acabado de leer del fichero lo cerramos
	/*
  A continuación estamos devolviendo un puntero a int al que se le hace un casting a void.
	Esto significa que estaremos devolviendo la dirección de memoria de un enter. Este entero
  se ha reservado en el montículo del proceso o hilo principal, que es accesible a todas los hilos creados.
  ¿Que significa la ultima frase?
  */		 
	pthread_exit((void*) nlines);//devolvemos el valor del numero de lineas con pthread_exit
}

int main(int argc, char const *argv[])
{  
  if (argc < 2)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error debe llamar al programa de la siguiente forma: ./E1 n (siendo n el numero de hebras que desea generar)");
    exit(EXIT_FAILURE);
  }
  int N = (argc-1);//N representa el numero de hilos que vamos a crear como es uno por cada fichero pasado como argumento lo igualamos al numero de argumentos -1 ya que la llamada al programa también cuenta como argumento
	pthread_t thread[N];//Creamos un vector para almacenar los identificadores de los hilos
	int *ret, suma = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, suma almacenará la suma de los valores devueltos por los hilos
  for (int i = 0; i < N; i++)
  {
    //En este bucle es donde creamos los n hijos que ejecutarán la función th_file
  	if(pthread_create(&(thread[i]), NULL, (void*) th_file, (void*) argv[i+1]))
    /*
    El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. 
    El segundo null es un puntero para los argumentos que le queramos pasar a la función que va a ejecutar la hebra
    Al hacer el bucle con n crearemos n hilos por eso usamos thread i ya que va de 0 a n-1
    Podemos reutilirar i para acceder a los nombres de los ficheros si le sumamos 1 ya que va a haber
    tambien n ficheros pero en este caso no queremos que vaya de 0 a n-1 queremos que empiece en 1
    porque el argv[0] es el nombre de este programa E1.c por eso le sumamos 1
    */
	  {
	    fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	    exit(EXIT_FAILURE); //Finalizamos la ejecución
	  }
  }
  for (int i = 0; i < N; i++)//Con este bucle recogemos a todas las hebras
  {
  	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
  	de un puntero a entero. Al pasar la dirección de memoria de un puntero por referencia, el contenido
  	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
  	memoria de un entero que se devuelve en "pthread_exit((void *)nlines)". Por tanto, "int * ret" ahora
  	apunta a donde apuntaba "int * nlines", solo que está casteado a void.*/
    if(pthread_join(thread[i], (void*) &ret)) 
	  {//Entramos si falla el join
	    fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	    exit(EXIT_FAILURE);//Salimos
	  }
    suma += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
  }     
  printf("La suma de las lineas de todos los ficheros es: %d\n", suma);//Imprimimos la suma
  exit(EXIT_SUCCESS);//Salimos indicando que todo ha ido bien
}
