#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
  const float pi = 3.1415926535897932384626433832795028f;
  const float twopi = 2.f * pi;
  printf("two pi: %.8x\n", *((unsigned int*) &twopi));
  float start = -2.f * pi;
  for(int idx = 0; idx <= 64; ++idx)
    {
      float angle = start + ((float) idx)* pi/16.f;
      angle = angle * 10.f;
      printf("z0_ = 32'h%.8x; #10; //cos(%f)=%f, sin(%f)=%f\n",
	     *((unsigned int*) &angle),
	     angle, cosf(angle),
	     angle, sinf(angle));
    }
  return 0;
}
