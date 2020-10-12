#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//La definición del main por defecto es char const * argv y hay que cambiarla a char * const argv[]

int main(int argc, char * const argv[]) //Al final del argv hay un puntero a null
{
  int status; //Creamos la variable que almacena el estado de salida del hijo
  pid_t pid, childpid; //Estas variables almacenan el id de los procesos hijos.
  //Pid almacena el valor devuelto al padre tras el fork y chilpid el valor devuelto al padre por la función wait cuando termina de esperar al hijo 
  printf("Soy %d el padre de todos\n", getpid());//El proceso padre imprime su id
  for (int i = 0; i < 2; i++)
  {
    pid=fork(); //Aqúi el proceso tiene su hijo. En el padre pid valdrá el id del hijo y en el hijo pid valdrá 0
    switch(pid) //En base al valor de pid cada proceso ejecutará una función
    {
      case 0://El fork se ha realizado corractamente
        printf("Soy %d el hijo nº %d del proceso: %d\n", getpid(), (i+1), getppid()); //El hijo se identifica
        if (i==0)//Como el programa pide que se ejecuten dos aplicaicones diferentes usaremos el valor de i para ejecutar una u otra
        {//Esto lo podemos hacer ya que el padre de ambos procesos es el mismo y el valor de i se mantiene en las distintas iteraciones
          execlp(argv[1], argv[1], NULL); //La p permite buscar la ruta asi que habria que usar los exec con p de path
        } //Repetimos el argv ya que uno es la ruta y el otro el ejecutable
        else
        { //argv+2 es para pasarle los argumentos a gedit
            execvp(argv[2], argv+2);
        }
        exit(EXIT_SUCCESS);//Cuando finaliza la ejecución los hijos mueren ¿Esto es necesario?

      case -1:
        printf("Error al crear el proceso hijo\n");//Se informa al usuario
        exit(EXIT_FAILURE); //Indica que ha ocurrido un fallo en la ejecución 
      
      default:
        printf("Esperando a que acabe mi hijo nº %d\n", i+1);
    }
  }
  while ( (childpid=wait(&status)) > 0 ) 
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
  exit(EXIT_SUCCESS); //Como todo ha ido bien el proceso padre acaba exitosamente
}