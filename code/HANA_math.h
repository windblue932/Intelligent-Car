#ifndef __MATH_H__
#define __MATH_H__

#include "zf_common_headfile.h"



#define isignof(IntVar) ((IntVar)<0?-1:1)//��ֵ����
#define isignOrZero(a) ((a)<0?-1:(a)>0?1:0)//��ֵ���ţ������㣬С���㣬�㣩

#ifndef ABS
#define ABS(a) (((a)>0) ? (a):(-1*(a)))
#endif

#ifndef max
#define max(a,b)        ((a) > (b) ? (a) : (b))//�����
#endif

#ifndef min
#define min(a,b)        ((a) < (b) ? (a) : (b))//����С
#endif

#ifndef LABS
#define LABS(a)	((a)>=0 ? (a) : -(a))//����ֵ
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) :( b))
#endif


//�޷�
#define LIMIT2MAX( a,b)( (a) = (a)>(b)?(b):(a))
#define LIMIT2MIN( a,b)( (a) = (a)<(b)?(b):(a))
#define LIMIT(val,minV,maxV) ((val)=((val)>(maxV))?(maxV) : ( ((val)<(minV)) ? (minV) : (val)))
#define LIMIT2(val,minV,maxV) (((val)>(maxV))?(maxV) : ( ((val)<(minV)) ? (minV) : (val)))

//Ԫ�ظ���
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof( (a) ) / sizeof( *(a) ))
#endif

//���Բ�ֵ
#ifndef ABS_DIFF
#define ABS_DIFF(a,b) (((a)>(b)) ? ((a)-(b)) : ((b)-(a)))
#endif

//����ţ������㣩
#ifndef SIGN_OF
#define SIGN_OF(a)   ((a)<0?-1:(a)>0?1:0)
#endif

//����
#ifndef SWAP
#define SWAP(a, b) ((a)^=(b),(b)^=(a),(a)^=(b))
#endif

#define M_2PI 6.283185307179586  //2��
#define M_PI_YI  3.141592653589793f  //��
#define M_PI_ER 1.570796326794897f  //��/2

#define DEG_TO_RAD 0.017453292519943295769236907684886f  //��->rad
#define RAD_TO_DEG 57.295779513082320876798154814105f   //rad->��

int my_abs(int value);
float my_abs_float(float in);
short my_abs_short(short in);
float invSqrt(float x); //���ټ��� 1/Sqrt(x)
float fast_atan(float v);//���ٷ�����
uint8_t lenOfNum(uint16_t num);
void leastSquareMethod(short* x, short* y, uint8_t len, float* k, float* b);
float getLeastSquareMethodERROR(short*x,short*y,uint8_t len,float k,float b);
float getLineValue(uint8_t x,float k,float b);
float getLineValueX(uint8_t x, uint8_t x1, short y1, float k);
float getLineK(short x1,short y1,short x2,short y2);
float constrain_float(float amt, float low, float high);//float�޷�
int16_t constrain_int16(int16_t amt, int16_t low, int16_t high);//16bit�޷�
int16_t constrain_int16_MAX(int16_t amt, int16_t high);
int16_t constrain_int16_LOW(int16_t amt, int16_t low);
int32_t constrain_int32(int32_t amt, int32_t low, int32_t high);
float radians(float deg);//�Ƕ�תrad
float degrees(float rad);//radת�Ƕ�
float sq(float v);//ƽ��
float pythagorous2(float a, float b);//2ά������
float pythagorous3(float a, float b, float c);//3ά������
int32_t wrap_360_cd(int32_t error);//0~360�޷���0-360��
float wrap_180_cd(float error);//-180~+180�޷���-180~180��
float wrap_90_cd(float error);//-90~90�޷���-90~90��
int limit_int(int a, int b, int c);
float limit_float(float a, float b, float c);


#endif
