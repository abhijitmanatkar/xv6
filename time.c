#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf(2, "Usage: time command args...\n");
    exit();
  }
  int pid = fork();
  if(pid < 0){
    printf(2, "time: Error forking\n");
    exit();
  }
  if(pid == 0){
    if(exec(argv[1], argv + 1) < 0){
      printf(2, "time: Couldn't execute command\n");
    }
  }
  else if(pid > 0){
    int wtime, rtime;
    waitx(&wtime, &rtime);
    printf(1, "Process PID %d:\nRunning time - %d clock cycles\nWaiting time - %d clock cycles\n", pid, wtime, rtime);
    exit();
  }
}