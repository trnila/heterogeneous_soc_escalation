#include <stdio.h>

volatile int num = 0x12345678;

int main() {
  int i = 0;
  uint8_t allowed = 0;
  for(;;) {
    if(allowed == 42) {
      printf("ALLOWED\r\n");
    } else {
      printf("%x\r\n", i++);
    }
  }
}
