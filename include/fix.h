//fix.h:
#ifndef FIX_H
#define FIX_H
 
#include <Arduino.h>
#include "stdint.h"
 
//Pragmas
#define FP_IBITS        16 //integer bits
#define FP_FBITS        15 //fraction bits
#define FP_BITS         32 //total bits (s16.15)
#define FP_MIN          -2147450880L
#define FP_MAX          2147450880L
#define FP_FMASK        (((int32_t)1<<FP_FBITS) - 1)
#define FP_ONE          ((int32_t)0x8000)
#define FP_CONST(R)     ((int32_t)((R)*FP_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define FP_PI           FP_CONST(3.14159265358979323846)
#define FP_TWO_PI       FP_CONST(2*3.14159265358979323846)
#define FP_HALF_PI      FP_CONST(3.14159265358979323846/2)
#define FP_ABS(A)       ((A) < 0 ? -(A) : (A))
#define FP_FRAC_PART(A) ((int32_t)(A)&FP_FMASK)
#define FP_DegToRad(D)  (FP_Divide(D, (int32_t)1877468))
#define FP_RadToDeg(R)  (FP_Multiply(R, (int32_t)18529868))
#define FP_FMOD(A, B)   ((A) % (B))
 
//conversion Functions
#define itok(i)         ( (int32_t)( (int32_t)i<<(int32_t)FP_FBITS ) )
#define ktoi(k)         ( ( (int16_t)( (int32_t)k>>(int32_t)FP_FBITS ) )&0x0000ffff )
#define ftok(f)         ( (int32_t)(float)( (f)*(32768) ) )
extern float FP_FixedToFloat(int32_t);
extern int32_t FP_FloatToFixed(float);
int32_t FP_StringToFixed(char *s);
void FP_FixedToString(int32_t f, char *s);
 
//basic math
extern int32_t FP_Multiply(int32_t, int32_t);
extern int32_t FP_Divide(int32_t, int32_t);
 
//special functions
int32_t FP_Round(int32_t, uint8_t);
 
//square root
extern int32_t _FP_SquareRoot(int32_t, int32_t);
#define FP_Sqrt(a)      _FP_SquareRoot((a), 15);
 
//trig
extern int32_t FP_Sin(int32_t);
#define FP_Cos(A)       (FP_Sin(FP_HALF_PI - A))
#define FP_Tan(A)       (FP_Divide(FP_Sin(A), FP_Cos(A)))

//reverse trig
extern int32_t FP_Acos(int32_t x);
#define FP_Asin(A)      (FP_HALF_PI - FP_Acos((A))) 

#endif //FIX_H