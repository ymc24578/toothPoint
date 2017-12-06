#pragma once
// #ifndef   OCTREE_H_FILE       //如果没有定义这个宏   
// #define   OCTREE_H_FILE       //定义这个宏   

// -----------------------------------------------------------------------------
// This defines a callback for traversal
// -----------------------------------------------------------------------------
#include <string>

//class   Octree;
//typedef bool            (*callback)(const Octree &o, void *data);
////注意,这里不是声明函数指针,而是定义一个函数指针的类型,这个类型是自己定义的,类型名为callback
// -----------------------------------------------------------------------------
// This defines a point class (it's incomplete, but the data elements are there)
// -----------------------------------------------------------------------------


 struct   Point
{
        double          x, y, z;        // Position
        //double          n;              // User's unique identifier
        unsigned char    code;           // Used during octree generation

		Point(double ix=0.0,double iy=0.0,double iz=0.0,unsigned char co =0)
		{
			x = ix;
			y = iy;
			z = iz;
			code = co;
		}
		Point& operator  = (const Point& v)
		{ x  = v.x; y  = v.y; z  = v.z; return *this; }
		void operator += (const Point& v)
		{ x += v.x; y += v.y; z += v.z; }
		void operator -= (const Point& v)
		{ x -= v.x; y -= v.y; z -= v.z; }
		//矢量加
		Point operator + (const Point& v) const
		{ return Point(x + v.x, y + v.y, z + v.z); }
		//矢量减
		Point operator - (const Point& v) const
		{ return Point (x - v.x, y - v.y, z - v.z); }
		//矢量乘(点积)
		double  operator * (const Point& v) const
		{ return x * v.x + y * v.y + z * v.z; }
		//数乘
		Point operator * (double num) const
		{ return Point (x * num, y * num, z * num); }
		double GetLength2()
		{ return (x*x+y*y+z*z); }
	
        // Insert typical operators, such as *, +, -, etc.
};

// -----------------------------------------------------------------------------
// This defines a cubic bounding volume (center, radius)
// -----------------------------------------------------------------------------

 struct Bounds
{
        Point           center;         // Center of a cubic bounding volume
        double          radius;         // dimension of a cubic bounding volume,when use should be
										// multi be 0.5
		Bounds(Point p=Point(0,0,0,0),double r=0.0)
		{
			center = p;
			radius = r;
		}
} ;

// -----------------------------------------------------------------------------
// The octree class -- almost real code!
// -----------------------------------------------------------------------------

class   Octree
{
public:
        // Construction/Destruction

                                Octree();
virtual                         ~Octree();

        // Accessors

inline  const   Point * const * points() const {return _points;}
inline  const   unsigned int    pointCount() const {return _pointCount;}

        // Implementation

  bool            build(Point **points,
                                      const unsigned int count,
                                      const unsigned int threshold,
                                      const unsigned int maximumDepth,
                                      const Bounds &bounds,
                                      const unsigned int currentDepth = 0);

Bounds calcCubicBounds(const Point * const * points,const unsigned int count);
//virtual const bool  traverse(callback proc, void *data) const;
void FindMinDistance( const Point &sp, Point &cp, double &rs);
// sp: source point.	cp: current closest point. rs: current closest distance square
bool IsInnerNode();
bool IsOutsideRadius(const Point &sp, const double &r);
//static unsigned int GetComputeCount(){return computeCount;}

public:
        Octree                  *_child[8];		// 8 children
        unsigned int            _pointCount;	// how many points in the node
        Point                   **_points;		// array of point pointers
//         Point                   _center;		// center of the node
//         double                  _radius;		
		Bounds					bbox;
		//static unsigned int		computeCount;

};


 // #endif  