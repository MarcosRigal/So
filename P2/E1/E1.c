#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

void * th_rand ()//Esta será la función que ejecuten las hebras.
{
	float x = (float)rand()/(float)(RAND_MAX);//Generamos y asignamos un valor aleatorio para x
  //(float) es un casting a float para que al dividir tengamos los decimales 
  printf("x: %f\n", x);//Imprimimos el valor de x tal y como nos lo pide el enunciado
  float y = (float)rand()/(float)(RAND_MAX);//Generamos y asignamos un valor aleatorio para y
  //RAND_MAX es un numero grandisimo y vale 2147483647 es para que nos salgan decimales chiquitos
  printf("y: %f\n", y);//Imprimimos el valor de y tal y como nos lo pide el enunciado
  float * z = malloc(sizeof(float));//reservamos memoria para un puntero de tipo float
  *z = x+y;//Guardamos en z el valor de la suma de x e y
  printf("x+y: %f\n", *z);//Imprimimos el valor de z tal y como nos lo pide el enunciado
	/*
  A continuación estamos devolviendo un puntero a float al que se le hace un casting a void.
	Esto significa que estaremos devolviendo la dirección de memoria de un flotante. Este flotante
  se ha reservado en el montículo del proceso o hilo principal, que es accesible a todas los hilos creados.
  ¿Que significa la ultima frase?
  */		 
	pthread_exit((void *)z);//devolvemos el valor de la suma con pthread_exit
}

int main(int argc, char const *argv[])
{  
  if (argc < 2)//Comprobamos que se haya llamado al programa de forma adecuada
  {
    printf("Error debe llamar al programa de la siguiente forma: ./E1 n (siendo n el numero de hebras que desea generar)");
    exit(EXIT_FAILURE);
  }
  srand(time(NULL));//Generamos la semilla para los números aleatorios
  int N = atoi(argv[1]);//Convertimos a entero argv[1] que continene el número de hilos que queremos generar
	pthread_t thread[N];//Creamos un vector para almacenar los identificadores de los hilos
	float *ret, suma = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, suma almacenará la suma de los valores devueltos por los hilos
  for (int i = 0; i < N; i++)
  {
    //En este bucle es donde creamos los n hijos que ejecutarán la función th_rand
  	if(pthread_create(&(thread[i]), NULL, (void*) th_rand, NULL))
    /*
    El primer NULL es para definir si la hebra se incia en modo nucleo o en modo usuario al poner null es modo nucleo. 
    El segundo null es un puntero para los argumentos que le queramos pasar a la función que va a ejecutar la hebra
    */
	  {
	    fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	    exit(EXIT_FAILURE); //Finalizamos la ejecución
	  }
  }
  for (int i = 0; i < N; i++)//Con este bucle recogemos a todas las hebras
  {
  	/* A pthread_join() le estamos pasando por referencia como segundo parámetro la dirección
  	de un puntero a float. Al pasar la dirección de memoria de un puntero por referencia, el contenido
  	de ese puntero (a donde apunta) puede ser modificado. Con la dirección de
  	memoria de un flotante que se devuelve en "pthread_exit((void *)z)". Por tanto, "float * ret" ahora
  	apunta a donde apuntaba "float * z", solo que está casteado a void.*/
    if(pthread_join(thread[i], (void*) &ret)) 
	  {//Entramos si falla el join
	    fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	    exit(EXIT_FAILURE);//Salimos
	  } 
    suma += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
  }     
  printf("La suma de todos es: %f\n", suma);//Imprimimos la suma
  exit(EXIT_SUCCESS);//Salimos indicando que todo ha ido bien
}