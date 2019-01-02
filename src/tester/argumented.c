#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
  while(1) {
    for (int i = 0; i < argc; ++i) {
      puts(argv[i]);
      sleep(1);
    }
  }

  return 0;
}
