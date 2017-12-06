
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
// ��άʸ��
class Vector2D	
{
public:
	Scalar	x, y;

public:
	//���캯��
	Vector2D ()
		{ x = y = 0; }
	Vector2D (Scalar newx, Scalar newy)
		{ x = newx; y = newy; }

	//��ֵ������
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

	//ʸ����
	Vector2D operator + (const Vector2D& v) const
		{ return Vector2D (x + v.x, y + v.y); }
	//ʸ����
	Vector2D operator - (const Vector2D& v) const
		{ return Vector2D (x - v.x, y - v.y); }
	//ʸ����(���)
	Scalar  operator * (const Vector2D& v) const
		{ return x * v.x + y * v.y; }
	//����
	Vector2D operator * (Scalar num) const
		{ return Vector2D (x * num, y * num); }
	//����
	Vector2D operator / (Scalar num) const
		{
			ASSERT ( NONZERO(num) );
			return Vector2D (x / num, y / num); 
		}
	//��Ŀ��
	Vector2D operator - () const
		{ return Vector2D (-x, -y); }

	//���㳤�ȵ�ƽ��
	Scalar GetLength2 ()
		{ return (x*x+y*y); }
	//���㳤��
	Scalar GetLength ()
		{ return sqrt(GetLength2()); }
	//��Ϊ��λʸ��
	void SetUnit ()
		{
			Scalar a = GetLength();
			if ( a>TOLER ) { x/=a; y/=a; }
		}

	//�е�(����)
	BOOL operator == (const Vector2D& v) const
		{
			Scalar a = (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y);
			return (a<=TOLER);
		}
	BOOL operator != (const Vector2D& v) const
		{ return !(operator==(v)); }

	//ȡԪ��
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
// ��άʸ��
class Vector3D
{
public:
	Scalar	x, y, z;

public:
	//���캯��
	Vector3D ()
		{ x = y = z = 0; }
	Vector3D (Scalar newx, Scalar newy, Scalar newz)
		{ x = newx; y = newy; z = newz; }

	//��ֵ������
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

	//ʸ����
	Vector3D operator + (const Vector3D& v) const
		{ return Vector3D (x + v.x, y + v.y, z + v.z); }
	//ʸ����
	Vector3D operator - (const Vector3D& v) const
		{ return Vector3D (x - v.x, y - v.y, z - v.z); }
	//ʸ����(���)
	Scalar  operator * (const Vector3D& v) const
		{ return x * v.x + y * v.y + z * v.z; }
	//ʸ����(���)
	Vector3D operator ^ (const Vector3D& v) const
		{ return Vector3D ( y * v.z - z * v.y,
						    z * v.x - x * v.z,
							x * v.y - y * v.x); 
		}
	//����
	Vector3D operator * (Scalar num) const
		{ return Vector3D (x * num, y * num, z * num); }
	//����
	Vector3D operator / (Scalar num) const
		{
			ASSERT ( NONZERO(num) );
			return Vector3D (x / num, y / num, z / num); 
		}
	//��Ŀ��
	Vector3D operator - () const
		{ return Vector3D (-x, -y, -z); }

	//���㳤�ȵ�ƽ��
	Scalar GetLength2 ()
		{ return (x*x+y*y+z*z); }
	//���㳤��
	Scalar GetLength ()
		{ return sqrt(GetLength2()); }
	//��Ϊ��λʸ��
	void SetUnit ()
		{
			Scalar a = GetLength();
			if ( a>TOLER ) { x/=a; y/=a; z/=a; }
		}

	//�е�(����)
	BOOL operator == (const Vector3D& v) const
		{
			Scalar a = (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z);
			return (a<=TOLER);
		}
	BOOL operator != (const Vector3D& v) const
		{ return !(operator==(v)); }

	//ȡԪ��
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
// ����άʸ��

class Vector
{
public:
	Scalar	*data;
	// ʸ����ά��
	int size;

public:
	//���캯��
	Vector ()
		{ data = NULL; size = 0; }
	Vector (int n);
	Vector (const Vector& v);
	~Vector ()
		{ if ( data ) delete data; }
	void Create (int n);
	void Create (const Vector& v);
	void Destroy ();

	//��ֵ������
	void operator  = (const Vector& v);
	void operator += (const Vector& v);
	void operator -= (const Vector& v);
	void operator *= (Scalar num);
	void operator /= (Scalar num);

	//���㳤�ȵ�ƽ��
	Scalar GetLength2 ();
	//���㳤��
	Scalar GetLength ();
	//��Ϊ��λʸ��
	void SetUnit ();
	//ȫ��Ϊ��ͬ��ֵ
	void SetAll (Scalar num);
	//ȫ��ȡ��(�൱��"*=-1")
	void SetNegative ();

	//�е�(����)
	//BOOL operator == (const Vector& v) const;
	//BOOL operator != (const Vector& v) const;
	
	//ȡԪ��
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

