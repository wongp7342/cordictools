#include "fixedpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int nlz(unsigned int x);
static unsigned int clz16(unsigned short x);
static unsigned int clz32(unsigned int x);
static unsigned int clz64(unsigned long long x);

void CopyFixedPoint(union FixedPointNumber* dest, const union FixedPointNumber* src)
{
  dest->data = src->data;
}

float CreateFloatFromFixedPointNumber(const union FixedPointNumber* pfixedp)
{
	unsigned int data = pfixedp->data;
	union SinglePrecision spfloat;
	spfloat.data = 0;

	
	if(!!(data & 0x80000000))
	  {
		data = -data;
		spfloat.fields.sign = 1;		
	  }
	unsigned int leadingzeros = nlz(data);// clz32(data);
	spfloat.fields.exp = ((4-1) - leadingzeros) + 127;
	spfloat.fields.frac = ((data << (leadingzeros + 1))) >> (32-23);

	return spfloat.num;
}

void PrintBin(unsigned long long val, unsigned int width)
{
  unsigned int idx = 0;
  unsigned long long mask = ((unsigned long long)1) << ((width - 1));
  for(; idx < width; ++idx)
    {
      printf("%d", !!(val & mask));
      val <<= 1;
    }
  printf("\n");
}

void CreateFixedPointNumber(float num, union FixedPointNumber* pfixedp)
{
	
	union SinglePrecision floatp;
	memset(&floatp, 0, sizeof(union FixedPointNumber));
	floatp.num = num;
	unsigned int frac = floatp.fields.frac;
	unsigned int exp = floatp.fields.exp;
	int shamt = exp - 127;

	pfixedp->data = shamt < 0 ? ((0x00800000 | frac) << 5) >> (-shamt):
	  ((0x00800000 | frac) << 5) << shamt;
	pfixedp->data = floatp.fields.sign ? (~pfixedp->data + 1) : pfixedp->data;
}



static int nlz(unsigned int x)
{
  /* Goryavsky's nlz, a variation on Harley's algorithm for finding
     the # of leading zeros in an integer */
  
	static char table[64] =
			{ 32, 20, 19, 255, 255, 18, 255, 7, 10, 17, 255, 255, 14, 255, 6,
					255, 255, 9, 255, 16, 255, 255, 1, 26, 255, 13, 255, 255,
					24, 5, 255, 255, 255, 21, 255, 8, 11, 255, 15, 255, 255,
					255, 255, 2, 27, 0, 25, 255, 22, 255, 12, 255, 255, 3, 28,
					255, 23, 255, 4, 29, 255, 255, 30, 31 };
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x & ~(x >> 16);
	x = x * 0xfd7049ff;
	return (int) table[x >> 26];
}

static unsigned int clz16(unsigned short x)
{
	if(!x)
		return 16;
	unsigned int n = 0;
	unsigned int mask = 0xFF00;
	for(unsigned int shift = 8; shift; )
	{
		if(!(x & mask))
		{
			n += shift;
			x <<= shift;
		}
		shift >>= 1;
		mask <<= shift;
	}
	return n;
}


static unsigned int clz32(unsigned int x)
{
	if(!x)
		return 32;
	unsigned int n = 0;
	unsigned int mask = 0xFFFF0000;
	for(unsigned int shift = 16; shift; )
	{
		if(!(x & mask))
		{
			n += shift;
			x <<= shift;
		}
		shift >>= 1;
		mask <<= shift;
	}
	return n;
}


static unsigned int clz64(unsigned long long x)
{
	if(!x)
		return 64;
	unsigned int n = 0;
	unsigned long long mask = 0xFFFFFFFF00000000;
	for(unsigned int shift = 32; shift; )
	{
		if(!(x & mask))
		{
			n += shift;
			x <<= shift;
		}
		shift >>= 1;
		mask <<= shift;
	}
	return n;
}

void PrintFixedAsFloat(const char* label, const union FixedPointNumber* num)
{
  float temp = CreateFloatFromFixedPointNumber(num);
  printf("%s: %f\n", label, temp);
}
