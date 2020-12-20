#include <sys/types.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <signal.h>

//Para testear si un exec funciona ponemos una linea despues del exec si se ejecuta es que el exec ha fallado
//Añadir al factorial una impresion parcial del factorial

int main(int argc, char const *argv[])
{
  if (argc < 2)//Comprobamos que se haya llamado al programa de forma adecuada
   {
      printf("Error falta el numero de hijos que se desea crear.\n");
      printf("Llame al programa de la siguiente forma: (./E5 n) donde n es el numero de hijos que desea crear \n");
      exit(EXIT_FAILURE);
   }

   int id_Memoria;//Variable para almacenar el identificador de memoria compartida
	int id_Semaforo;//Variable para almacenar el identificador de memoria compartida del semaforo
   int *contador = NULL;//Puntero a la zona de memoria compartida
	sem_t *mutex = NULL;//Semaforo para bloquear el acceso al contador
   int value;//Almacena el valor devuelto por shmdt al realizar la desconexión

   key_t Clave = ftok("Extra2.c",3); //Usamos ftok para generar la clave de acceso a la zona de memoria compartida
   if (Clave == -1) //Comprobamos que la clave se haya generado correctamente
   {
      perror("Error al generar la clave");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));  
		exit(EXIT_FAILURE);
   }

   id_Memoria = shmget (Clave, sizeof(int), IPC_CREAT | SHM_R | SHM_W);//Solicitamos un segmento de memoria compartida
	if (id_Memoria == -1)//Comprobamos que se la solicitud se haya completado satisfactoriamente
	{
		printf("Main() de E1... No consigo ID para la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

   contador = (int *)shmat(id_Memoria, NULL, 0);//Enganchamos el segmento con el proceso
	if (contador == NULL)
	{
		printf("Main() de demo1... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	id_Semaforo = shmget (Clave+1, sizeof(sem_t), IPC_CREAT | SHM_R | SHM_W);//Solicitamos un segmento de memoria compartida
	if (id_Semaforo == -1)//Comprobamos que se la solicitud se haya completado satisfactoriamente
	{
		printf("Main() de E1... No consigo ID para la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	mutex = (sem_t *)shmat(id_Semaforo, NULL, 0);//Enganchamos el segmento con el proceso
	if (mutex == NULL)
	{
		printf("Main() de demo1... No consigo enlace a la memoria compartida.\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

  	int status; //Creamos la variable que almacena el estado de salida del hijo
  	pid_t pid, childpid; //Estas variables almacenan el id de los procesos hijos.
	sem_init(mutex, 1, 1);
  	//Pid almacena el valor devuelto al padre tras el fork y chilpid el valor devuelto al padre por la función wait cuando termina de esperar al hijo 
  	printf("Soy %d el padre de todos\n", getpid());//El proceso padre imprime su id
  	char arg1[256];
	char arg2[256];
	sprintf(arg1, "%d", id_Memoria);
	sprintf(arg2, "%d", id_Semaforo);
	for (int i = 0; i < atoi(argv[1]); i++)
  	{
    pid=fork(); //Aqúi el proceso tiene su hijo. En el padre pid valdrá el id del hijo y en el hijo pid valdrá 0
    switch(pid) //En base al valor de pid cada proceso ejecutará una función
    {
      case 0://El fork se ha realizado corractamente
        printf("Soy %d el hijo nº %d del proceso: %d\n", getpid(), (i+1), getppid()); //El hijo se identifica
        execlp("./Contador2", "./Contador2", arg1, arg2,NULL);//La p permite buscar el ejecutable en el path y en el directorio de trabajo
        exit(EXIT_SUCCESS);

      case -1:
        printf("Error al crear el proceso hijo\n");//Se informa al usuario
        exit(EXIT_FAILURE); //Indica que ha ocurrido un fallo en la ejecución 

      default:
        printf("Esperando a que acabe mi hijo nº %d\n", i+1);
    }
  }
  while ( (childpid=waitpid(-1, &status, WUNTRACED | WCONTINUED)) > 0 )//Si lo hacemos así en vez de con wait podemos saber si el proceso ha sido pausado y poniendo -1 en el primer parametro de waitpid esperamos a cualquier hijo
	{//Este bucle se repetirá mientas haya hijos que esperar cuando no haya mas wait devolverá -1
		if (WIFEXITED(status)) 
		{//Entrará en el caso de que el hijo haya finaizado correctamente ya que WIFEXITED(status) devolverá true
			printf("Proceso padre %d, hijo con PID %ld finalizado, status = %d\n", getpid(), (long int)childpid, WEXITSTATUS(status));
		} 
		else if (WIFSIGNALED(status))
		{//Entrará en el caso de que el proceso haya finalizado debido a una señar externa ya sea de finalizar o matar 
			printf("Proceso padre %d, hijo con PID %ld finalizado al recibir la señal %d\n", getpid(), (long int)childpid, WTERMSIG(status));
		}//La macro WTERMSIG nos dice que señal ha sido la que ha recibido el proceso que ha producido que acabe 
	}
	if (childpid==(pid_t)-1 && errno==ECHILD) 
	{//Entra cuando vuelve al while y no hay más hijos que esperar porque en ese caso chilpid valdrá -1 y erno 10 que es el valor que devuelce ECHILD cuando no hay mas procesos hijo
		printf("Proceso padre %d, no hay mas hijos que esperar. Valor de errno = %d, definido como: %s\n", getpid(), errno, strerror(errno));
	}	//strerror devuelve una cadena de caracteres que nos permite identificar el valor de la variable errno
	else
	{//Solo entra si se ha producido un error con wait 
		printf("Error en la invocacion de wait o waitpid. Valor de errno = %d, definido como: %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE); //Acaba el proceso padre con error
	}
  	printf("El valor de la variable es: %d\n", *contador);//Imprimimos el valor de la variable
   value = shmctl (id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);//Como no vamos a volver a usar la memoria compartida, la marcamos para borrado.
   if (value == -1)//Comprobamos el valor devuelto por shmctl
	{
		printf("Error en shmctl...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	 value = shmctl (id_Semaforo, IPC_RMID, (struct shmid_ds *)NULL);//Como no vamos a volver a usar la memoria compartida, la marcamos para borrado.
   if (value == -1)//Comprobamos el valor devuelto por shmctl
	{
		printf("Error en shmctl...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
   value = shmdt ((char *)contador);//Desconectamos el proceso padre de la zona de memoria compartida
   if (value == -1)//Comprobamos el valor de la desconexion
	{
		printf("Error en shmdt...\n");
		printf("Valor de errno=%d, definido como %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
  exit(EXIT_SUCCESS); //Como todo ha ido bien el proceso padre acaba exitosamente
}