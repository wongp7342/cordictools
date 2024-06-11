#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  printf("Enter fixed point in hex:");
  union FixedPointNumber num;
  scanf("%x", &num.data);
  
  PrintFixedAsFloat("Result", &num);
  return 0;
}
