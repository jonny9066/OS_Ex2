#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void our_signal_hndler (int signum) {
  printf("\n called with signal %d", signum);
}

int main () {
  
  signal(SIGINT, our_signal_hndler);

  while (1) {
    printf("\n Try to kill me:)!");
    usleep(1000000);// 1 sec
  }
}