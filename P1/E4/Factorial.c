#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int factorial(int n)
{//Funcion para calcular el factorial del numero pasado como argumento
  int resultado = 1;//Inicializamos la variable resultado a 1
  for (int i = n; i > 0; i--)
  {//En cada iteración del bucle el proceso se duerme 1 segundo
    sleep(1);
    printf("%d\n", resultado);//Aquí imprime el resultado parcial del factorial
    resultado *=i;
  }
  
  return resultado;//Devuelve el factorial del numero introducido
}

int main(int argc, char const *argv[])
{
  if (argc<2)
  {//Comprueba que el numero de parámetos introducido sea el correcto
    printf("Error, debe pasar un numero como argumento para calcular su factorial");
  }//Llama a la función factorial e imprime el resultado
  printf("El factorial de %s es: %d\n", argv[1],factorial(atoi(argv[1])));
  exit(EXIT_SUCCESS);//Como todo ha ido bien el proceso acaba exitosamente
}
