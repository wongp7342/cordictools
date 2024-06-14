#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



int main(void)
{
  const float PI = 3.14159265358979323846264338f;
  union FixedPointNumber pid2, twopi, pi, threepidiv2;
 CreateFixedPointNumber(PI/2.f, &pid2);
 CreateFixedPointNumber(2.f*PI, &twopi);
 CreateFixedPointNumber(PI, &pi);
 CreateFixedPointNumber(1.5f*PI, &threepidiv2);
 printf("PI/2: 0x%.8x\n", pid2.data);
 printf("2PI: 0x%.8x\n", twopi.data);
 printf("PI: 0x%.8x\n", pi.data);
 printf("1.5*PI: 0x%.8x\n", threepidiv2.data); 
		       
  return 0;
}
