#include <stdio.h>
#include <stdlib.h>

int num_Ok(int n)
{
  if (n==2)
  {
    return 1;
  }
  if (n==5)
  {
    return 1;
  }
  return 0;
}
//fichero sample ok
int main(int argc, char const *argv[])
{
  if (argc < 2 || num_Ok(atoi(argv[1]))==0)
  {
    printf("Error llame al programa de la siguiente forma:\n");
    printf("./E3 2\n");
    printf("o:\n");
    printf("./E3 5\n");
  }
  


  return 0;
}
