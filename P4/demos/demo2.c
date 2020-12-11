//
//	Programa 2 para demostración de Memoria compartida.
//

/* 
Este programa demo2.c habilita el acceso a una zona de memoria compartida
ya creada por el programa demo1.c, a la cual ambos procesos podrán acceder.
Se leeran numeros del 0 al 9 que escribe el proceso demo1.

demo1 debe ser arrancado primero, porque es el que crea la zona de memoria
compartida. Si demo2 se arranca antes se indicara error.

Los accesos estan controlados con simples sleep(), se deberia utilizar semaforos.
*/

#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//La misma clave que la indicada en demo1.c. En este caso no se ha usado ftok().
#define Clave (key_t) 234  
#define MAX 10

int main()
{
	int id_Memoria;
	int *memoria = NULL;
	int i,value;
    
	//	Igual que en demo1.c, obtenemos el id de la memoria. Al no poner
	//	el flag IPC_CREAT en el ultimo parametro, estamos suponiendo que
	// dicha memoria ya está creada.
	
	//id_Memoria = shmget (Clave, sizeof(int)*10, 0660 );
	id_Memoria = shmget (Clave, sizeof(int)*10, SHM_R | SHM_W );
	if (id_Memoria == -1)
	{
		printf("Main() de demo2... No consigo ID para la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
    
	//
	//	Igual que en demo1.c, obtenemos un puntero a la memoria compartida
	//
	memoria = (int *)shmat (id_Memoria, (char *)0, 0);
	if (memoria == NULL)
	{
		printf("Main() de demo2... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
    
	//
	//	Vamos leyendo el valor de la memoria con esperas de un segundo
	//	y mostramos en pantalla dicho valor. Debería ir cambiando según
	//	p1 lo va modificando.
	//
	for (i=0; i<10; i++)
	{
	   printf("Soy demo 2 - Leido %d de la posicion %d\n",memoria[i],i);

 		/* Con este sleep() simularemos la sincronizacion sin que haya
		condiciones de carreras. ESTO ES UNA MALA PRACTICA DE PROGRAMACION,
		TOMELO COMO ALGO ILUSTRATIVO Y DOCENTE */
		sleep (2);
	}
    
	//
	//	Desasociamos nuestro puntero de la memoria compartida. Suponemos
	//	que p1 (el proceso que la ha creado), la liberará.
	//
	value = shmdt ((char *)memoria);
	if (value == -1)
	{
		 printf("Error en shmdt...\n");
		 printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		 exit(EXIT_FAILURE);
	}
}
