#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void printid()
{
  printf("Mi id es: %d\n", getpid());
}

int main()
{
  printf("Mi id es: %d\n", getpid());
  signal(SIGUSR1, printid);
  pause();
  return 0;
}
