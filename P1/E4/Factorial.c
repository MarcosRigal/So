#include <unistd.h>
#include <string.h>
#include <stdio.h>
int ctoi(const char * arg)
{
  return (*arg-'0');
}

int factorial(int n)
{
  int resultado = 1;
  for (int i = n; i > 0; i--)
  {
    sleep(1);
    resultado *=i;
  }
  
  return resultado;
}

int main(int argc, char const *argv[])
{
  if (argc<2)
  {
    printf("Error, debe pasar un numero como argumento para calcular su factorial");
  }
  printf("El factorial es: %d\n", factorial(ctoi(argv[1])));
  return 0;
}
