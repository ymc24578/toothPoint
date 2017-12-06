
#ifndef	_VECTOR_H_
#define _VECTOR_H_

#include "Global.h"
//#include <stdio.h>
//#include <math.h>

//#define TOLER 0.0000001

class Vector2D;
class Vector3D;
class Vector;



//---------------------------------------------------------
// 二维矢量
class Vector2D	
{
public:
	Scalar	x, y;

public:
	//构造函数
	Vector2D ()
		{ x = y = 0; }
	Vector2D (Scalar newx, Scalar newy)
		{ x = newx; y = newy; }

	//赋值操作符
	Vector2D& operator  = (const Vector2D& v)
		{ x  = v.x; y  = v.y; return *this; }
	void operator += (const Vector2D& v)
		{ x += v.x; y += v.y; }
	void operator -= (const Vector2D& v)
		{ x -= v.x; y -= v.y; }
	void operator *= (Scalar num)
		{ x *= num; y *= num; }
	void operator /= (Scalar num)
		{ 
			ASSERT ( NONZERO(num) );
			x /= num; y /= num; 
		}

	//矢量加
	Vector2D operator + (const Vector2D& v) const
		{ return Vector2D (x + v.x, y + v.y); }
	//矢量减
	Vector2D operator - (const Vector2D& v) const
		{ return Vector2D (x - v.x, y - v.y); }
	//矢量乘(点积)
	Scalar  operator * (const Vector2D& v) const
		{ return x * v.x + y * v.y; }
	//数乘
	Vector2D operator * (Scalar num) const
		{ return Vector2D (x * num, y * num); }
	//数除
	Vector2D operator / (Scalar num) const
		{
			ASSERT ( NONZERO(num) );
			return Vector2D (x / num, y / num); 
		}
	//单目减
	Vector2D operator - () const
		{ return Vector2D (-x, -y); }

	//计算长度的平方
	Scalar GetLength2 ()
		{ return (x*x+y*y); }
	//计算长度
	Scalar GetLength ()
		{ return sqrt(GetLength2()); }
	//设为单位矢量
	void SetUnit ()
		{
			Scalar a = GetLength();
			if ( a>TOLER ) { x/=a; y/=a; }
		}

	//判等(不等)
	BOOL operator == (const Vector2D& v) const
		{
			Scalar a = (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y);
			return (a<=TOLER);
		}
	BOOL operator != (const Vector2D& v) const
		{ return !(operator==(v)); }

	//取元素
	Scalar& operator [] (int i)
		{
			switch (i) 
			{
				case 0: return x;
				case 1: return y;
				default: return x;
			}
		}
};

//---------------------------------------------------------
// 三维矢量
class Vector3D
{
public:
	Scalar	x, y, z;

public:
	//构造函数
	Vector3D ()
		{ x = y = z = 0; }
	Vector3D (Scalar newx, Scalar newy, Scalar newz)
		{ x = newx; y = newy; z = newz; }

	//赋值操作符
	Vector3D& operator  = (const Vector3D& v)
		{ x  = v.x; y  = v.y; z  = v.z; return *this; }
	void operator += (const Vector3D& v)
		{ x += v.x; y += v.y; z += v.z; }
	void operator -= (const Vector3D& v)
		{ x -= v.x; y -= v.y; z -= v.z; }
	void operator *= (Scalar num)
		{ x *= num; y *= num; z *= num; }
	void operator /= (Scalar num)
		{
			ASSERT ( NONZERO(num) );
			x /= num; y /= num; z /= num; 
		}
	void operator ^= (const Vector3D& v)
		{ 
			Scalar a =   y * v.z - z * v.y;
			Scalar b = - x * v.z + z * v.x;
			Scalar c =   x * v.y - y * v.x;
			x = a;	y = b;	z = c;
		}

	//矢量加
	Vector3D operator + (const Vector3D& v) const
		{ return Vector3D (x + v.x, y + v.y, z + v.z); }
	//矢量减
	Vector3D operator - (const Vector3D& v) const
		{ return Vector3D (x - v.x, y - v.y, z - v.z); }
	//矢量乘(点积)
	Scalar  operator * (const Vector3D& v) const
		{ return x * v.x + y * v.y + z * v.z; }
	//矢量乘(叉积)
	Vector3D operator ^ (const Vector3D& v) const
		{ return Vector3D ( y * v.z - z * v.y,
						    z * v.x - x * v.z,
							x * v.y - y * v.x); 
		}
	//数乘
	Vector3D operator * (Scalar num) const
		{ return Vector3D (x * num, y * num, z * num); }
	//数除
	Vector3D operator / (Scalar num) const
		{
			ASSERT ( NONZERO(num) );
			return Vector3D (x / num, y / num, z / num); 
		}
	//单目减
	Vector3D operator - () const
		{ return Vector3D (-x, -y, -z); }

	//计算长度的平方
	Scalar GetLength2 ()
		{ return (x*x+y*y+z*z); }
	//计算长度
	Scalar GetLength ()
		{ return sqrt(GetLength2()); }
	//设为单位矢量
	void SetUnit ()
		{
			Scalar a = GetLength();
			if ( a>TOLER ) { x/=a; y/=a; z/=a; }
		}

	//判等(不等)
	BOOL operator == (const Vector3D& v) const
		{
			Scalar a = (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z);
			return (a<=TOLER);
		}
	BOOL operator != (const Vector3D& v) const
		{ return !(operator==(v)); }

	//取元素
	Scalar& operator [] (int i)
		{
			switch (i) 
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: return x;
			}
		}
};

//---------------------------------------------------------
// 任意维矢量

class Vector
{
public:
	Scalar	*data;
	// 矢量的维数
	int size;

public:
	//构造函数
	Vector ()
		{ data = NULL; size = 0; }
	Vector (int n);
	Vector (const Vector& v);
	~Vector ()
		{ if ( data ) delete data; }
	void Create (int n);
	void Create (const Vector& v);
	void Destroy ();

	//赋值操作符
	void operator  = (const Vector& v);
	void operator += (const Vector& v);
	void operator -= (const Vector& v);
	void operator *= (Scalar num);
	void operator /= (Scalar num);

	//计算长度的平方
	Scalar GetLength2 ();
	//计算长度
	Scalar GetLength ();
	//设为单位矢量
	void SetUnit ();
	//全设为相同的值
	void SetAll (Scalar num);
	//全部取负(相当于"*=-1")
	void SetNegative ();

	//判等(不等)
	//BOOL operator == (const Vector& v) const;
	//BOOL operator != (const Vector& v) const;
	
	//取元素
	Scalar& operator [] (int i)
		{
			ASSERT( data && (i>=0) && (i<size) );
			return data[i];
		}
};

//---------------------------------------------------------

Scalar Length (Vector2D& v);
Scalar Length (Vector3D& v);

#define BaseXV2D Vector2D(1.0, 0.0)
#define BaseYV2D Vector2D(0.0, 1.0)
        
#define BaseXV3D Vector3D(1.0, 0.0, 0.0)
#define BaseYV3D Vector3D(0.0, 1.0, 0.0)
#define BaseZV3D Vector3D(0.0, 0.0, 1.0)

//---------------------------------------------------------

typedef Vector2D	Point2D;
typedef	Vector3D	Point3D;

Scalar Distance (const Point2D& p1, const Point2D& p2);
Scalar Distance (const Point3D& p1, const Point3D& p2);

Point2D MidPoint (const Point2D& p1, const Point2D& p2);
Point3D MidPoint (const Point3D& p1, const Point3D& p2);

Point2D MinPoint (const Point2D& p1, const Point2D& p2);
Point3D MinPoint (const Point3D& p1, const Point3D& p2);
Point2D MaxPoint (const Point2D& p1, const Point2D& p2);
Point3D MaxPoint (const Point3D& p1, const Point3D& p2);

#define ZeroP2D Point2D(0.0, 0.0)
#define ZeroP3D Point3D(0.0, 0.0, 0.0)
        
#include "vector.inl"

#endif // _VECTOR_H_

