#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Command {
  int index;
  pid_t pid;
};

char * separator = "%";
int argStart = 1;
int cmdCount = 0;
int cmdArrayCount = 1;
struct Command * commands;

void getSeparator(char * arg) {
  if (arg[0] == '-' && arg[1] == 's') {
    if (arg[2] == 0) {
      fputs("Expected a separator\n", stderr);
      exit(EXIT_FAILURE);
    }

    separator = arg + 2;
    argStart++;
  }
}

void createCommand(int index) {
  if (argStart == index) {
    argStart++;
    return;
  }

  if (cmdCount == cmdArrayCount) {
    cmdArrayCount <<= 1;
    commands = realloc(commands, cmdArrayCount * sizeof(struct Command));
    if (!commands) {
      perror("Failed to allocate memory for command strings");
      exit(EXIT_FAILURE);
    }
  }

  commands[cmdCount].index = argStart;

  cmdCount++;
  argStart = index + 1;
}

int main(int argc, char * argv[]) {

  if (argc < 2) {
    fputs("Expected an executable\n", stderr);
    exit(EXIT_FAILURE);
  }

  getSeparator(argv[1]);
  printf("Separator: %s\n", separator);

  commands = malloc(sizeof(struct Command));
  if (!commands) {
    perror("Failed to allocate memory for command strings");
    exit(EXIT_FAILURE);
  }

  for (int i = argStart; i < argc; ++i) {
    if (strcmp(argv[i], separator) == 0) {
      argv[i] = 0;
      createCommand(i);
    }
  }
  createCommand(argc);

  if (cmdCount == 0) {
    fputs("Expected an executable\n", stderr);
    exit(EXIT_FAILURE);
  }

#ifdef VERBOSE
  for (int i = 0; i < cmdCount; ++i) {
    printf("Command %d:", i);
    for (int j = commands[i].index; j < argc; ++j) {
      if (argv[j] == 0) {
        break;
      }
      printf(" %s", argv[j]);
    }
    puts("");
  }
#endif

  for (int i = 0; i < cmdCount; ++i) {
#ifdef VERBOSE
    printf("[%s] Forking\n", argv[commands[i].index]);
#endif

    pid_t pid = fork();

    if (pid < 0) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
#ifdef VERBOSE
      printf("[%s] Starting\n", argv[commands[i].index]);
#endif
      free(commands);
      execv(argv[commands[i].index], argv + commands[i].index + 1);
    } else {
      printf("[%s] Started with PID %ld\n", argv[commands[i].index], (long) pid);
      commands[i].pid = pid;
    }
  }

  for (int i = 0; i < cmdCount; ++i) {
    int status = 0;

#ifdef VERBOSE
    printf("[%s] Waiting for PID %ld\n", argv[commands[i].index], (long) commands[i].pid);
#endif
    waitpid(commands[i].pid, &status, 0);
    printf("[%s] Exited with status %d\n", argv[commands[i].index], status);
  }

  free(commands);
  exit(EXIT_SUCCESS);
}
