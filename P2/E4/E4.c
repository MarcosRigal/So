
#include <pthread.h>

int global=0;

void * incrementa()
{

  pthread_exit(NULL);
}

int main()
{
  pthread_t hilo1, hilo2;
  if (pthread_create())
  {
    
  }
  


  return 0;
}
