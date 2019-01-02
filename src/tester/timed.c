#include <stdio.h>

int main() {
  for (int i = 0; i < 5; ++i) {
    printf("Hey %d!\n", i);
    sleep(2);
  }

  return 0;
}
