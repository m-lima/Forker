#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define VERBOSE

char ** extractCommands(int argc, char * argv[], int * outputCmdCnt) {
  int argStart = 1;
  char * separator = "%";

  if (argv[1][0] == '-' && argv[1][1] == 's') {
    if (argv[1][2] == 0) {
      fputs("[31m -- Expected a separator[m\n", stderr);
      exit(EXIT_FAILURE);
    }

    separator = argv[1] + 2;
    argStart++;
  }

#ifdef VERBOSE
  printf("[35m -- Separator: %s[m\n", separator);
#endif

  char ** cmds = malloc(2);
  if (!cmds) {
    perror("[31m -- Failed to allocate memory for commands strings[m");
    exit(EXIT_FAILURE);
  }

  int cmdAlloc = 2;
  int cmdCnt = 0;

  cmds[cmdCnt] = malloc(256);
  if (!cmds[cmdCnt]) {
    perror("[31m -- Failed to allocate memory for argument buffer[m");
    exit(EXIT_FAILURE);
  }

  int bufferAlloc = 256;
  int bufferLength = 0;
  cmds[cmdCnt][0] = 0;

  for (int i = argStart; i < argc; i++) {
    size_t argLength = strlen(argv[i]);
    if (argLength == 0) {
      continue;
    }

    if (strcmp(argv[i], separator) == 0) {
      if (bufferLength > 0) {
        if (cmdCnt >= cmdAlloc) {
          cmds = realloc(cmds, cmdAlloc + 2);
          cmdAlloc += 2;
        }

        cmdCnt++;
        cmds[cmdCnt] = malloc(256);
        if (!cmds[cmdCnt]) {
          perror("[31m -- Failed to allocate memory for argument buffer[m");
          exit(EXIT_FAILURE);
        }

        bufferAlloc = 256;
        bufferLength = 0;
        cmds[cmdCnt][0] = 0;
      }
      continue;
    }

    if (argLength + bufferLength + 1 >= bufferAlloc) {
      cmds[cmdCnt] = realloc(cmds[cmdCnt], bufferAlloc + 256);
      if (!cmds[cmdCnt]) {
        perror("[31m -- Failed to allocate memory for argument buffer[m");
        exit(EXIT_FAILURE);
      }
      bufferAlloc += 256;
    }

    if (bufferLength != 0) {
      strcat(cmds[cmdCnt], " ");
      bufferLength++;
    }

    strcat(cmds[cmdCnt], argv[i]);
    bufferLength += argLength;
  }

  *outputCmdCnt = cmdCnt + (cmds[cmdCnt][0] == 0 ? 0 : 1);
  return cmds;
}

int main(int argc, char * argv[]) {

  if (argc < 2) {
    fputs("[31m -- Expected an executable[m\n", stderr);
    exit(EXIT_FAILURE);
  }

  int cmdCnt = 0;
  int pidCnt = 0;
  char ** cmds = extractCommands(argc, argv, &cmdCnt);

  if (cmdCnt == 0) {
    fputs("[31m -- Expected an executable[m\n", stderr);
    exit(EXIT_FAILURE);
  }

  pid_t * pids = malloc(cmdCnt * sizeof(pid_t));

#ifdef VERBOSE
  for (int i = 0; i < cmdCnt; ++i) {
    printf("[35m -- Command %d: %s[m\n", i, cmds[i]);
  }
#endif

  for (int i = 0; i < cmdCnt; ++i) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("[31m -- Failed to fork[m");
      break;
    } else if (pid == 0) {
      exit(system(cmds[i]));
    } else {
      printf("[35m -- Started '%s' with PID %ld[m\n", cmds[i], (long) pid);
      pids[pidCnt] = pid;
      pidCnt++;
    }
  }

  for (int i = 0; i < pidCnt; ++i) {
    int status = 0;
    waitpid(pids[i], &status, 0);
    printf("[35m -- '%s' exited with status %d[m\n", cmds[i], status);
  }

  exit(EXIT_SUCCESS);
}
