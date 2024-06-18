#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <quadmath.h>

#include "fixedpoint.h"
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


int main(void)
{

  /* We are only interested in single precision here. See cordictest.c
     for half, double, and quad precision calculations */
  union FixedPointNumber zero;
  zero.data = 0;
  PrintFixedAsFloat("zero", &zero);
  CreateFixedPointNumber(0.f, &zero);
  PrintFixedAsFloat("zero", &zero);
  
	union FixedPointNumber *fxatans = (union FixedPointNumber*) malloc(sizeof(union FixedPointNumber) * 32);
	/* Populate table of fixed point precomputed atan values */
	for(int idx = 0; idx < 32; ++idx)
	{
	    float atansp = (float) atan2(1.0, ((double)(1 << idx)));
	    CreateFixedPointNumber(atansp, &fxatans[idx]);
	    printf("atantable(%d) := %u.U //Q4.28 fixed point of %f\n", idx, fxatans[idx].data, atansp);
	}
	
	union FixedPoint64* fxatans64 = (union FixedPoint64*) malloc(sizeof(union FixedPoint64) * 32);
	/* Populate table of fixed point precomputed atan values */
	for(int idx = 0; idx < 32; ++idx)
	{
	    float atansp = (float) atan2(1.0, ((double)(1 << idx)));
	    FloatToFixed64(atansp, &fxatans64[idx]);
	    printf("atantable(%d) := scala.BigInt(\"%.16llx\", 16).U(64.W) //Q32.32 fixed point of %f\n", idx, 
	    fxatans64[idx].data, atansp);
	}
	
	for(int idx = 0; idx < 32; ++idx)
	{
		union FixedPoint64* fxnum = &fxatans64[idx];
		float num = Fixed64ToFloat(fxnum);
		printf("Reading back %d: %f\n", idx, num);
	}
	
	/* k is the limit of the product of the sequence i = 0 to n of cos(1/(2^i)) as n -> inf */
	/* We pre-scale the unit vector for the CORDIC algorithm by
	   dividing it by k, which saves us two multiplications at the
	   end */
	const double k = 0.60725293500888125616944675;
	const float kf = (float) k;
	float invk = 1.f/kf;	
	const double PI = 3.141592653589793238462643383279502884197169;

	float x = kf, y = 0.f;
	printf("Starting with [%f %f]\n", x, y);
	float alpha = PI/2.f; //45.0 * PI/180.0;
	float theta = 0.0;

	union FixedPointNumber fxk, fxinvk;
	CreateFixedPointNumber(k, &fxk);
	CreateFixedPointNumber(1.f/k, &fxinvk);	
	printf("k Q4.28: %u.U\n", fxk.data);
	printf("invk Q4.28: %u.U\n", fxinvk.data);
	
	union FixedPointNumber fxalpha, fxtheta, fxsigma, fxx, fxy;
	CreateFixedPointNumber(alpha, &fxalpha);
	CreateFixedPointNumber(theta, &fxtheta);
	CreateFixedPointNumber(x, &fxx);
	CreateFixedPointNumber(y, &fxy);

	
	PrintFixedAsFloat("fxx", &fxx);
	PrintFixedAsFloat("fxy", &fxy);
	PrintFixedAsFloat("fxtheta", &fxtheta);

	for(int i = 0; i < 32; ++i)
	{
		union FixedPointNumber fxt; // temporary for storing copy of x
		CopyFixedPoint(&fxt, &fxx);
		union FixedPointNumber fxxterm, fxyterm, fxatan;
		CopyFixedPoint(&fxxterm, &fxt);
		CopyFixedPoint(&fxyterm, &fxy);
		CopyFixedPoint(&fxatan, &fxatans[i]);
		
		PrintFixedAsFloat("fxatans[i]", &fxatans[i]);		

		if(fxtheta.data >= fxalpha.data)
		  {
		    fxxterm.data = -fxxterm.data;
		    fxyterm.data = -fxyterm.data;
		    fxatan.data = -fxatan.data;
		  }
		
		fxtheta.data += fxatan.data;
		fxyterm.data = fxyterm.data >> i;
		fxxterm.data = fxxterm.data >> i;
		fxx.data = fxt.data - (fxyterm.data);
		fxy.data = fxy.data + (fxxterm.data);
		PrintFixedAsFloat("fxx", &fxx);
		PrintFixedAsFloat("fxy", &fxy);
		PrintFixedAsFloat("fxtheta", &fxtheta);		
	}
	

	float recoveredx = CreateFloatFromFixedPointNumber(&fxx);
	float recoveredy = CreateFloatFromFixedPointNumber(&fxy);	

	printf("Recovered x: %f y: %f\n", recoveredx, recoveredy);
	printf("Recovered data: cos: %.8x sin: %.8x\n", *((unsigned int*) &recoveredx), *((unsigned int*) &recoveredy));
	float softcosf = cosf(alpha);
	float softsinf = sinf(alpha);

	printf("software single precision: cos: %f, sin: %f\n",
	       softcosf, softsinf);
	printf("software 32-bit data: cos: %.8x sin: %.8x\n",
	       *((unsigned int*) &softcosf),
	       *((unsigned int*) &softsinf));
	
	free(fxatans);
	fxatans = 0;
	return 0;
}
