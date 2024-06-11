#include <stdio.h>
#include <string.h>

union DoublePrecision
{
struct DPFPFields
{
  unsigned long long frac : 52;
  short exp : 11;  
  unsigned char sign : 1;
} fields;
  double num;
};

void printbin(unsigned long long val, unsigned int width)
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

void printfpfields(struct DPFPFields* fp)
{
  printbin(fp->sign, 1);
  printbin(fp->exp, 11);
  printbin(fp->frac, 52);
}

union SinglePrecision
{
	struct FPFields
	{
	  unsigned int frac : 23;
	  unsigned int exp : 8;  
	  unsigned int sign : 1;
	} fields;
  float num;
  unsigned int data;
};

union FixedPointNumber
{
	//Q4.28
	struct
	{
		unsigned char i : 4;
		unsigned int  f : 28;
	} fields;
	unsigned int data;
};

void CreateFixedPointNumber(float num, union FixedPointNumber* pfixedp)
{
	
	union SinglePrecision floatp;
	memset(&floatp, 0, sizeof(union FixedPointNumber));
	floatp.num = num;
	unsigned int frac = floatp.fields.frac;
	unsigned int exp = floatp.fields.exp;
	int shamt = exp - 127;
	printf("Frac: ");
	printbin(frac, 32);
	printf("shamt: %d\n", shamt);
	pfixedp->data = shamt < 0 ? ((0x00800000 | frac) << 5) >> (-shamt):
	  ((0x00800000 | frac) << 5) << shamt;
	pfixedp->data = floatp.fields.sign ? (~pfixedp->data + 1) : pfixedp->data;
}

int nlz(unsigned int x)
{
	//Goryavsky's nlz, a variation of Harley's algorithm
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

unsigned int clz16(unsigned short x)
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


unsigned int clz32(unsigned int x)
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


unsigned int clz64(unsigned long long x)
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

	printf("Leading zeroes: %d\n", leadingzeros);

	spfloat.fields.frac = ((data << (leadingzeros + 1))) >> (32-23);
	printf("Data: ");	
	printbin(data, 32);
	
	
	printf("Float from fixed:");
	printbin(spfloat.data, 32);

	return spfloat.num;
}

int main(void)
{

	float input = 0.f;
	printf("Enter float:");
	scanf("%f", &input);
	if(input >= 7.f || input <= -7.f)
	  printf("Input is out of range of Q4.28.\n");
	union SinglePrecision sp;
	sp.num = input;
	printf("Float %f: ", sp.num);
	printbin(sp.data, 32);

	union FixedPointNumber fxnum;
	memset(&fxnum, 0, sizeof(union FixedPointNumber));
	
	CreateFixedPointNumber(sp.num, &fxnum);
	printf("0x%.8x\n", fxnum.data);
	printf("Fixed point: ");
	printbin(fxnum.data, 32);
	printf("\n");
	printf("Leading 0s of 10: %d\n", clz64(10));
	float result = CreateFloatFromFixedPointNumber(&fxnum);
	printf("Converted back: %f\n", result);
	/*
	
  union DoublePrecision testnum;
  testnum.num = 4.5;
  unsigned int testint = 5;
  printbin(testint, 32);
  printf("\n%llx\n", *((unsigned long long*)(&(testnum.num))));
  printfpfields(&(testnum.fields));
  testnum.fields.exp -= 1;
  testnum.fields.sign = ~testnum.fields.sign;
  printf("After adding 1 to exp %f\n", testnum.num);
  return 0;
  */
}
