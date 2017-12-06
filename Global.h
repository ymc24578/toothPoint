
#ifndef	_GLOBAL_H_
#define _GLOBAL_H_

#include <math.h>

#ifndef BOOL
#define BOOL   int
#define TRUE   1
#define FALSE  0
#endif


#ifndef NULL
#define NULL 0
#endif

#ifndef ASSERT
#define ASSERT(X) 
#endif

// 一些常量定义
#undef PI//lcj
#define PI			3.14159265358979323846
#define	PI2			6.28318530717958647692
#define PI_2		1.57079632679489661923
#define PI_4		0.785398163397448309616
#define PI_8		0.392699081698724154808
#define PI_16		0.196349540849362077404
#define PI_3		1.04719755119659774615
#define PI_6		0.523598775598298873077
#define E			2.71828182845904523536
#define LOG2E		1.44269504088896340736
#define LOG10E		0.434294481903251827651
#define LN2			0.693147180559945309417
#define LN10		2.30258509299404568402
#define SQRT2		1.41421356237309504880
#define SQRT2_2		0.707106781186547524401
#define SQRT3		1.732050807568877293527
#define SQRT3_2     0.8660254037844386467637
#define SQRT5		2.236067977499789696409

#define DEGREE			(PI / 180.0)
#define	RAD(angle)		((angle)*PI/180.0)

//#define MAX_INT		2147483647
#define MAX_FLOAT		((3.402823466e+38F) / 4)

// 与零比较的容差
#define TOLER			0.0000001
// 零/非零判断
#define ISZERO(a)		((a>-TOLER)&&(a<TOLER))
#define NONZERO(a)		((a>TOLER)||(a<-TOLER))

// 最大/最小/中值
#define MAX(a,b)		(((a)>(b))?(a):(b))
#define MIN(a,b)		(((a)<(b))?(a):(b))
#define MAX3(a,b,c)		MAX(MAX(a,b),c)
#define MIN3(a,b,c)		MIN(MIN(a,b),c)
#define MID3(a,b,c)		(a+b+c-MAX3(a,b,c)-MIN3(a,b,c))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

// 两个数交换
#define SWAP(x,y)		{x+=y;y=x-y;x-=y;}

// 由COLORREF格式颜色得到R、G、B分量 (0 - 255)
#define R(color)			(GetRValue(color))
#define G(color)			(GetGValue(color))
#define B(color)			(GetBValue(color))
// 由COLORREF格式颜色得到R、G、B分量 (0.0 - 1.0)
#define Rf(color)			((float)R(color)/255.0f)
#define Gf(color)			((float)G(color)/255.0f)
#define Bf(color)			((float)B(color)/255.0f)

// 用于下标
//#define		X	0
//#define		Y	1
//#define		Z	2

//typedef double Scalar;
typedef float Scalar;

inline BOOL IsZero(Scalar a)
{
	return (a>-TOLER) && (a<TOLER);
}
inline BOOL IsNotZero(Scalar a)
{
	return (a>TOLER) || (a<-TOLER);
}
inline BOOL IsEqual(Scalar a, Scalar b)
{
	return IsZero(a-b);
}
inline BOOL IsNotEqual(Scalar a, Scalar b)
{
	return IsNotZero(a-b);
}
inline Scalar ScalarMin(Scalar a, Scalar b)
{
	return (a < b) ? a : b;
}
inline Scalar ScalarMax(Scalar a, Scalar b)
{
	return (a > b) ? a : b;
}
inline Scalar ScalarMin(Scalar a, Scalar b, Scalar c)
{
	return ScalarMin(ScalarMin(a, b), c);
}
inline Scalar ScalarMax(Scalar a, Scalar b, Scalar c)
{
	return ScalarMax(ScalarMax(a, b), c);
}
inline Scalar ScalarMid(Scalar a, Scalar b, Scalar c)
{
	return (a<b) ? ((b<c)?b:ScalarMax(a,c)) : ((a<c)?a:ScalarMax(b,c));
}
inline void ScalarMinMax(Scalar a, Scalar b, Scalar& mn, Scalar& mx)
{
	mn = ScalarMin(a, b);
	mx = ScalarMax(a, b);
}
inline void ScalarMinMax(Scalar a, Scalar b, Scalar c, Scalar& mn, Scalar& mx)
{
	mn = ScalarMin(a, b, c);
	mx = ScalarMax(a, b, c);
}
inline Scalar ScalarLength(Scalar a, Scalar b)
{
	return sqrt(a * a + b * b);
}
inline Scalar ScalarLength2(Scalar a, Scalar b)
{
	return a * a + b * b;
}
inline Scalar ScalarLength(Scalar a, Scalar b, Scalar c)
{
	return sqrt(a * a + b * b + c * c);
}
inline Scalar ScalarLength2(Scalar a, Scalar b, Scalar c)
{
	return a * a + b * b + c * c;
}

#endif // _GLOBAL_H_
