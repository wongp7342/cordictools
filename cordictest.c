#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <quadmath.h>
// gcc cordic.c -o cordic -lm -msse2 -lquadmath

void FPAdder(double a, double b, double* out)
{
  *out = a + b;
}

void IntAdd(unsigned int a, unsigned int b, unsigned int* out)
{
  *out = a + b;
}

void IntSub(unsigned int a, unsigned int b, unsigned int* out)
{
  *out = a - b;
}

void FPSub(double a, double b, double* out)
{
  *out = a - b;
}

void Mux(unsigned long long a, unsigned long long b, unsigned int sel,
	 unsigned long long* out)
{
  if(sel)
    {
      *out = b;
    }
  else
    {
      *out = a;
    }
}

union WrappedDouble
{
struct FPFields
{
  unsigned long long frac : 52;
  signed short exp : 11;  
  unsigned char sign : 1;
} fields;
  double num;
};


union qpunion
{
     __float128 qpnum;
      struct
      {
	unsigned int word1, word2, word3, word4;
      } fields;
};

int main(void)
{
	_Float16 testfloat = 6.28f;
	printf("Half precision float: %f\n", (float) testfloat);
	  printf("Half precision data: %u\n", *((unsigned short*) &testfloat));
	/* The number of iterations we do is equal
	   to the number of accurate bits we get */
     	unsigned int n = 32;
	double* atans = (double*) malloc(sizeof(double) * n);

	for(int idx = 0; idx < 128; ++idx)
	  {
	    __float128 atanqp = atan2q(1.0, ((__float128)(1 << idx)));
	    char buffer[2048] = {0};

	    union qpunion temp;
	    temp.qpnum = atanqp;
	    quadmath_snprintf(buffer, 2048, "%Qe", 30, atanqp);
	    printf("atantable(%d) := scala.BigInt(\"%.8x%.8x%.8x%.8x\", 16).U(128.W) //%s\n",
		   idx, temp.fields.word4, temp.fields.word3, temp.fields.word2, temp.fields.word1, buffer);
	  }
	
	/* Build list of arctans. This would go in a ROM or lookup table? */
	for(int idx = 0; idx < 66; ++idx)
	{
	  atans[idx] = atan2(1.0, ((double)(1 << idx)));
	  /* printf("atan[%d] dp: %lf (%lld)\n", idx, atans[idx],
	   *((unsigned long long*) &atans[idx])); */
	  printf("atantable(%d) := scala.BigInt(\"%llu\", 10).U(64.W) //%lf\n", idx, *((unsigned long long*) &atans[idx]), atans[idx]);	  
	}

	for(int idx = 0; idx < n + 2; ++idx)
	  {
	    float atansp = (float) atan2f(1.0, ((float)(1 << idx)));
	    printf("atantable(%d) := %u.U //%f\n", idx, *((unsigned long*) &atansp), atansp);
	    /*printf("atan[%d] sp: %f (%d)\n", idx, atansp,
	     *((unsigned long*) &atansp));*/
	  }

	for(int idx = 0; idx < 18; ++idx)
	  {
	    _Float16 atanhp = (_Float16) atan2(1.0, ((double)(1 << idx)));
	    printf("atantable(%d) := %u.U //%f\n", idx, *((unsigned short*) &atanhp), (float) atanhp);
	  }
	
	

	/* k is the limit of the product of the sequence i = 0 to n of cos(1/(2^i)) as n -> inf */
	const double k = 0.6072529350088812561694;
	const float kf = (float) k;
	const _Float16 hpk = (_Float16) k;
	const __float128 qpk = 0.60725293500888125616944675250492826311239085215008977245697601311014788120842490690622742590803840527499484512597405104579976519q;
	const double PI = 3.141592653589793238462643383279502884197169;
	union qpunion temp;
	temp.qpnum = qpk;
	printf("size of __float128: %d kqp mem: 0x%.8x %.8x %.8x %.8x\n", sizeof(__float128), temp.fields.word4, temp.fields.word3, temp.fields.word2, temp.fields.word1);
	printbin(temp.fields.word4, 32);
	printbin(temp.fields.word3, 32);
	printbin(temp.fields.word2, 32);
	printbin(temp.fields.word1, 32);
	printf("\n");
	printf("size of double: %d k mem: %lld\n", sizeof(double), (*(unsigned long long*) &k));
	printf("size of float: %d kf mem: %d\n", sizeof(float), (*(unsigned int*) &kf));
	printf("sizes of _Float16: %d kf mem: %u\n", sizeof(_Float16), (*(unsigned short*) &hpk));
	float invk = 1.f/k;
	_Float16 invkhp = (_Float16) invk;
	double invkdp = 1.0/k;
	__float128 invkqp = 1.0q/qpk;
	temp.qpnum = invkqp;
	
	printf("16-bit reciprocal of k: %d\n", *((unsigned short*) &invkhp));
	printf("32-bit reciprocal of k: %ld\n", *((unsigned int*) &invk));
	printf("64-bit reciprocal of k: %lld\n", *((unsigned long long*) &invkdp));
	printf("128-bit reciprocal of k: 0x%.8x%.8x%.8x%.8x\n", temp.fields.word4, temp.fields.word3, temp.fields.word2, temp.fields.word1);
	
	double x = kf, y = 0.0;
	printf("Starting with [%f %f]\n", x, y);
	double alpha = 45.0 * PI/180.0;
	double theta = 0.0;
	double sigma = 1.0;

	for(int i = 0; i < n; ++i)
	{
		double t = x;
		union WrappedDouble sigy, sigx;
		sigy.num = y;
		sigx.num = t;
	  
		sigma = theta >= alpha ? -1.0 : 1.0;
		if(theta >= alpha)
		{
			sigx.fields.sign =  ~sigx.fields.sign;
			sigy.fields.sign =  ~sigy.fields.sign;
		}
		theta += sigma * atans[i];
		unsigned int sigyexp = 0, sigxexp = 0;
		IntSub(sigy.fields.exp, i, &sigyexp);
		IntSub(sigx.fields.exp, i, &sigxexp);
		sigy.fields.exp = sigyexp;
		sigx.fields.exp = sigxexp;

		FPSub(x, sigy.num, &x);
		FPAdder(sigx.num, y, &y);
		printf("Step %d -- x: %f, y: %f, theta: %f\n", i, x, y, theta);
	}
	x = x * k;
	y = y * k;
	printf("cos: %f sin: %f\n", x, y);
	printf("cos: %.8x sin: %.8x\n", *((unsigned int*) &x), *((unsigned int*) &y));
	float softcosf = cosf(alpha);
	float softsinf = sinf(alpha);
	printf("cos: %lf, sin: %lf\n", x, y);
	printf("software 32-bit cos: %f, sin: %f\n",
	       softcosf, softsinf);
	printf("soft 32-bit cos: %.8x sin: %.8x\n",
	       *((unsigned int*) &softcosf),
	       *((unsigned int*) &softsinf));
	printf("%f double precision data: %lld\n", alpha,
	*((unsigned long long*) &alpha));
	free(atans);
	atans = 0;
	return 0;
}
