#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static void sig_handler(int);

int
main () {

  int parent_pid, child_pid, status;
  if (SIG_ERR == signal(SIGUSR1, sig_handler)) {
    printf("Parent: Unable to create handler SIGUSR1\n");
  }
  if (SIG_ERR == signal(SIGUSR2, sig_handler)) {
    printf("Parent: Unable to create handler SIGUSR2\n");
  }
  parent_pid = getpid();

  /** fork() returns the child's pid to the parent process
   * and returns 0 to the child process
   * assuming it succeeds this code will skip over the if 
   * statement in the parent because fork will have returned
   * the child's pid which won't be 0 and the child will jump
   * into the if statement because from its point of view fork()
   * returned 0. */
  if ( 0 == (child_pid = fork()) ) {
    kill(parent_pid, SIGUSR1);
    for (;;) pause();
  } else {
    kill(child_pid, SIGUSR2);
    sleep(10);
    printf("Parent: Terminating child...\n");
    kill(child_pid, SIGTERM);
    printf("wait output = %d\n", wait(&status));
    printf("%d = status, and %d = child_pid\n", status, child_pid);
    printf("done\n");
  }
  
}

static void sig_handler(int signo) {
  switch (signo) {
  case SIGUSR1:
    printf("Parent: Received SIGUSR1\n");
    break;
  case SIGUSR2:
    printf("Child: Received SIGUSR2\n");
    break;
  default:
    break;
  }
  return;
}
