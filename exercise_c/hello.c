#include "stdio.h"

int main(int argc, char *argv[]) {
  if (argc==1){
    printf("Hello, World!\n");
  }
  if (argc==2){
    printf("Hello, %s!\n", argv[1]);
  }
  if (argc>2){
    printf("This program requires zero or one arguments!");
  }
}
