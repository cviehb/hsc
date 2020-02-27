#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: calc_atan <times>");
    return 1;
  }
  
  int len = atoi(argv[1]);
  double div = 1.0;
  for(int i = 0; i < len; i++) {
    printf("%.32f,\n", atan(div));
    div = div / 2;
  }
  
  return 0;
}

