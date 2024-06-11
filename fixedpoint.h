#ifndef FIXEDPOINT_H_
#define FIXEDPOINT_H_

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
	int data;
};

void CopyFixedPoint(union FixedPointNumber* dest, const union FixedPointNumber* src);
void PrintBin(unsigned long long val, unsigned int width);
void PrintFixedAsFloat(const char* label, const union FixedPointNumber* num);
void CreateFixedPointNumber(float num, union FixedPointNumber* pfixedp);
float CreateFloatFromFixedPointNumber(const union FixedPointNumber* pfixedp);


#endif /* FIXEDPOINT_H_ */
