//
//	Programa 1 para demostración de Memoria compartida.
//

/* 
Este programa demo1.c crea una zona de memoria compartida con el programa demo2.c,
a la cual ambos procesos podrán acceder.
El proceso demo1 escribira numeros del 0 al 9 que leerá el proceso demo2.

Los accesos estan controlados con simples sleep(), se deberia utilizar semaforos.
*/
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//	Conseguimos una clave para la Memoria compartida. Todos los
//	procesos que quieran compartir la Memoria, deben obtener la misma
//	clave. En este caso no se ha usado ftok().

#define Clave (key_t) 234

//  La clave también se puede conseguir por medio de la función ftok().
//	A esta función se le pasa un fichero cualquiera que exista y este
//	accesible (todos los procesos deben pasar el mismo fichero) y un
//	entero cualquiera (todos los procesos el mismo entero). El siguiente
//  código debería colocarse al principio del main (ver código comentado)

#define MAX 10

int main()
{
	//key_t Clave;
	int id_Memoria;
	int *memoria = NULL;
	int i, value;
    
    /* key_t clave = ftok (".", 33);
     if (clave == -1)
    {
        	perror("ftok error");
			printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));  
			exit(EXIT_FAILURE);
     }
     */
    
	//  Una vez tenemos la Clave:
	//	Creamos la Memoria con la clave recién conseguida. Para ello llamamos
	//	a la función shmget pasándole la clave, el tamaño de Memoria que
	//	queremos reservar (100 enteros en nuestro caso) y unos flags.
	//	Los flags son  los permisos de lectura/escritura/ejecucion 
	//	para propietario, grupo y otros (es el 660 en octal) y el 
	//	flag IPC_CREAT para indicar que cree la Memoria.
	//	La función nos devuelve un identificador para la Memoria recién
	//	creada.
	/*	 
	0660 esta en octal y significa lectura y escritura para
	el usuario, para el grupo al que pertenece el usuario y nada para el resto de usuarios: 
	                             user    group  others
	                             r w x   r w x  r w x
	                             1 1 0   1 1 0  1 1 0 
	            0 (octal)          6       6      0
	*/            
	id_Memoria = shmget (Clave, sizeof(int)*10, IPC_CREAT | SHM_R | SHM_W);
	//id_Memoria = shmget (Clave, sizeof(int)*10, IPC_CREAT | 0660);
	if (id_Memoria == -1)
	{
		printf("Main() de demo1... No consigo ID para la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/*	Una vez creada la Memoria, hacemos que uno de nuestros punteros
		apunte a la zona de Memoria recién creada. Para ello llamamos a
		shmat, pasándole el identificador obtenido anteriormente y un
		par de parámetros. 
	
	 - NULL asocia la memoria compartida a la primera direccion de memoria
	   que el sistema vea libre y adecuada.
	 
	 - 0 Es el tipo de uso que se va a hacer, asi lectura-escritura.
	 
	 Devuelve la direccion de inicio del segmento.
   */	 
	memoria = (int *)shmat(id_Memoria, NULL, 0);
	if (memoria == NULL)
	{
		printf("Main() de demo1... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}


	//	Ya podemos utilizar la Memoria.
	//	Escribimos cosas en la Memoria. Los números de 1 a 10 esperando
	//	un segundo entre ellos. Estos datos serán los que lea el otro
	//	proceso.
	for (i=0; i<10; i++)
	{
		memoria[i] = i;
		printf("Soy demo 1 - Escrito %d en la posicion %d\n",i,i);
		
		/* Con este sleep() simularemos la sincronizacion sin que haya
		condiciones de carreras. ESTO ES UNA MALA PRACTICA DE PROGRAMACION,
		TOMELO COMO ALGO ILUSTRATIVO Y DOCENTE */
		sleep (1); 
	}

	//	Terminada de usar la Memoria compartida, la marcamos para borrado.
   value = shmctl (id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
	if (value == -1)
	{
		printf("Error en shmctl...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	//	El proceso actual se desvincula de la memoria compartida.
	value = shmdt ((char *)memoria);
	if (value == -1)
	{
		printf("Error en shmdt...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}
