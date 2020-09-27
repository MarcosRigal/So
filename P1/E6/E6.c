#include <unistd.h>
#include <signal.h>
#include <stdio.h>
int i=0;

void RING()
{
  printf("RING\n");
  i++;
  if (i==4)
  {
    kill(getpid(), SIGKILL);
  }
  
}

int main(int argc, char const *argv[])
{
  signal(SIGALRM, RING);
  alarm(5);
  pause();
  alarm(3);
  pause();
  while (1)
  {
    alarm(1);
    pause();
  } 
  return 0;
}
