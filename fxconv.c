#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  printf("Enter fixed point in hex:");
  union FixedPoint64 num;
  scanf("%llx", &num.data);
  float result = Fixed64ToFloat(&num);
  printf("Result: %f\n", result);
  return 0;
}
