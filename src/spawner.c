#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
  if (argc < 2) {
    fputs("Expected an executable\n", stderr);
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; ++i) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      exit(system(argv[i]));
    } else {
      printf("Started '%s' with PID %ld\n", argv[i], (long) pid);
    }
  }

  for (int i = 1; i < argc; ++i) {
    int status = 0;
    wait(&status);
    printf("'%s' exited with status %d\n", argv[i], status);
  }

  exit(EXIT_SUCCESS);
}
