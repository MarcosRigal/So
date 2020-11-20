#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int impar = 0;
int par = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//Lo utilizo para bloquear el acceso al vector

int num_Ok(int num)//Comprobamos que el numero introducido por el usuario al llamar el programa sea > 0
{
  if (num>0)
  {
    return 1;//Como es > que 0 devolvemos 1 para que el programa continue con su ejecución
  }

  return 0;//Como no es > que 0  devolvemos 0 para que finalice la ejecución del programa
}

int num_Rand(){return ((rand()%11));}//Generamos numeros aleatorios del 0 al 10

int th_sum(void* orden)
{
  int *n = (int*) orden;
  int *suma = malloc(sizeof(int));//Reservamos memoria para un puntero de tipo int en el monticulo ya que sino desaparece
  *suma = 0;//Como lo vamos a usar de contador lo ponemos a 0
  int rand;//Variable para almacenar los numeros enteros
  if (*n%2==0)//Si esta hebra es par entramos
  {
    for (size_t i = 0; i < 5; i++)
    {
      rand = num_Rand();//Generamos un numero aleatorio
      *suma += rand; //Añadimos el numero a la variable local de la hebra
      pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de par
      par += rand;//Añadimos el numero a la variable global
      pthread_mutex_unlock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de par
    }
    pthread_exit((void*) suma);//Devolvemos la suma de los elementos del vector con pthread_exit
  }
  for (size_t i = 0; i < 5; i++)//El numero es impar
  {
    rand = num_Rand();//Generamos un numero aleatorio
    *suma += rand;//Lo sumamos a la variable local
    pthread_mutex_lock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de impar
    impar += rand;//Lo sumamos a la variable global
    pthread_mutex_unlock(&lock);//Semaforo para que las hebras no se pisen y modifiquen el valor de impar
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
  int nThreads = atoi(argv[1]);//Guardamos el numero de hilos
  int *ret, sumapar = 0, sumaimpar = 0;//ret es el puntero que recibirá los valores devueltos por los hilos, sumapar almacenará la suma de los valores devueltos por los hilos pares y sumaimpar hara lo propio con los impares
  pthread_t threads[nThreads];//Creamos un vector para almacenar los identificadores de los hilos de los clientes
  srand(time(NULL));//Generamos la semilla para los números aleatorios
  int contador[nThreads];//Este vector será el que nos dira si la variable es par o impar

  for (int i = 0; i < nThreads; i++)//Crearemos tantos hilos como haya indicado el usuario
  {
    contador[i]=i+1;
    /*Soy el Marcos del futuro que está comentando esto a las 1 am no se porque hice un vector si con pasar i+1 bastaba
    Pero no voy a cambiarlo porque es bastante probable que me lo cargue y tengo sueño
    Soy el Marcos del futuro + 10 minutos al final he probado a cambiarlo spoiler no funciona
    La gracia de utilizar el vector era pasarle valores constantes a la hebra que no se modifiquen 
    cuando se modifique i haciendo que sea independiente el orden de ejecución de cada hebra ya que 
    reciben un puntero a un valor estatico
    En este bucle es donde creamos los nThreads hilos que ejecutarán la función th_rand
  	*/
    if(pthread_create(&(threads[i]), NULL, (void*) th_sum, (void*) contador+(sizeof(int)*i)))
	  {
	    fprintf(stderr, "Error creating thread\n");//Ha ocurrido un error al crear la hebra
	    exit(EXIT_FAILURE); //Finalizamos la ejecución
	  }
  }

  for (int i = 0; i < nThreads; i++)//Con este bucle recogemos a todas las hebras y sus valores devueltos
  {
    if ((i+1)%2==0)//Si el hilo recibido tienen orden par entramos aqui
    {      
      if(pthread_join(threads[i], (void*) &ret)) 
	    {//Entramos si falla el join
	      fprintf(stderr, "Error joining thread\n");//Informamos al usuario
	      exit(EXIT_FAILURE);//Salimos
	    }
      printf("El valor devuelto por la hebra nº %d es: %d\n", i+1, *ret); 
      sumapar += *ret;//En cada iteración del bucle sumamos el valor devuelto por la hebra recogida
    }
    else//Si es impar entramos aquí
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
