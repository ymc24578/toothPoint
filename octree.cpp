
#include "./octree.h"


// -----------------------------------------------------------------------------
// Construction -- Just "nullify" the class
// -----------------------------------------------------------------------------

Octree::Octree()
: _pointCount(0), _points(NULL), /*_center(0,0,0,0), _radius(0.0),*/bbox(Point(0,0,0),0)
{
	memset(_child, 0, sizeof(_child));
	//		computeCount = 0;
}

// -----------------------------------------------------------------------------
// Destruction -- free up memory
// -----------------------------------------------------------------------------

Octree::~Octree()
{
	delete[] _points;
}

bool Octree::IsInnerNode()
{
	for (int i=0;i<8;i++)
	{
		if(_child[i]) return true;
	}
	return false;
}



bool Octree::IsOutsideRadius(const Point &sp, const double &rs)
{
	//check if the bounding box is outside the range

	double newrs;
	double r = bbox.radius*0.5;
	Point  p = bbox.center;

	// now test all the 8 corners
	//0 0 0 
	newrs = ((p.x-r)-sp.x)*((p.x-r)-sp.x) + 
		((p.y-r)-sp.y)*((p.y-r)-sp.y) + 
		((p.z-r)-sp.x)*((p.z-r)-sp.z);
	if(newrs<rs) return false;		

	//0 0 1
	newrs = ((p.x-r)-sp.x)*((p.x-r)-sp.x) + 
		((p.y-r)-sp.y)*((p.y-r)-sp.y) + 
		((p.z+r)-sp.x)*((p.z+r)-sp.z);
	if(newrs<rs) return false;	

	//0 1 0
	newrs = ((p.x-r)-sp.x)*((p.x-r)-sp.x) + 
		((p.y+r)-sp.y)*((p.y+r)-sp.y) + 
		((p.z-r)-sp.x)*((p.z-r)-sp.z);
	if(newrs<rs) return false;	

	//0 1 1
	newrs = ((p.x-r)-sp.x)*((p.x-r)-sp.x) + 
		((p.y+r)-sp.y)*((p.y+r)-sp.y) + 
		((p.z+r)-sp.x)*((p.z+r)-sp.z);
	if(newrs<rs) return false;	

	//1 0 0
	newrs = ((p.x+r)-sp.x)*((p.x+r)-sp.x) + 
		((p.y-r)-sp.y)*((p.y-r)-sp.y) + 
		((p.z-r)-sp.x)*((p.z-r)-sp.z);
	if(newrs<rs) return false;	

	//1 0 1
	newrs = ((p.x+r)-sp.x)*((p.x+r)-sp.x) + 
		((p.y-r)-sp.y)*((p.y-r)-sp.y) + 
		((p.z+r)-sp.x)*((p.z+r)-sp.z);
	if(newrs<rs) return false;	

	//1 1 0
	newrs = ((p.x+r)-sp.x)*((p.x+r)-sp.x) + 
		((p.y+r)-sp.y)*((p.y+r)-sp.y) + 
		((p.z-r)-sp.x)*((p.z-r)-sp.z);
	if(newrs<rs) return false;	

	//1 1 1
	newrs = ((p.x+r)-sp.x)*((p.x+r)-sp.x) + 
		((p.y+r)-sp.y)*((p.y+r)-sp.y) + 
		((p.z+r)-sp.x)*((p.z+r)-sp.z);
	if(newrs<rs) return false;	


	// outside
	return true;

}

// -----------------------------------------------------------------------------
// Build the octree
// -----------------------------------------------------------------------------

bool    Octree::build(Point **points,
					  const unsigned int count,
					  const unsigned int threshold,
					  const unsigned int maximumDepth,
					  const Bounds &bounds,
					  const unsigned int currentDepth)
{
	// You know you're a leaf when...
	//
	// 1. The number of points is <= the threshold
	// 2. We've recursed too deep into the tree
	//    (currentDepth >= maximumDepth)
	//
	//    NOTE: We specifically use ">=" for the depth comparison so that we
	//          can set the maximumDepth depth to 0 if we want a tree with
	//          no depth.

	// build bounding box for this node
	bbox = bounds;
	_pointCount = count;

	// current node point count
	if (_points==NULL)	// 有可能已经作为孩子节点被分配过内存了
	{
		_pointCount = count;
		_points = new Point*[_pointCount];
		for (int i=0;i<count;i++)
		{
			_points[i] = points[i];
		}
	}


	if (count <= threshold || currentDepth >= maximumDepth)
	{
		return true;
	}

	// We'll need this (see comment further down in this source)

	unsigned int    childPointCounts[8] = {0,0,0,0,0,0,0,0};


	// Classify each point to a child node

	for ( int i = 0; i < count; i++)
	{
		// Current point

		Point   &p = *(points[i]);		// this way, p is just a name, not copy

		// Center of this node

		const Point &c = bounds.center;

		// Here, we need to know which child each point belongs to. To
		// do this, we build an index into the _child[] array using the
		// relative position of the point to the center of the current
		// node

		p.code = 0;		// the last 3 bits
		if (p.x > c.x) p.code |= 1;
		if (p.y > c.y) p.code |= 2;
		if (p.z > c.z) p.code |= 4;
		// add one bit to flag dirty if  = 1, it is dirty


		// We'll need to keep track of how many points get stuck in each
		// child so we'll just keep track of it here, since we have the
		// information handy.

		childPointCounts[p.code]++;
	}

	// Recursively call build() for each of the 8 children

	for (int i = 0; i < 8; i++)
	{
		// Don't bother going any further if there aren't any points for
		// this child

		if (!childPointCounts[i]) continue;

		// Allocate the child

		_child[i] = new Octree;

		// Allocate a list of points that were coded JUST for this child
		// only

		_child[i]->_points = new Point*[childPointCounts[i]];

		// Go through the input list of points and copy over the points
		// that were coded for this child

		int cc = 0;
		Point   **ptr = _child[i]->_points;		// marker to fill in the newList
		for (int j = 0; j < count; j++)
		{
			if ((points[j]->code & 7) == i && (points[j]->code & 8) == 0 )	//the point belongs to the child
			{
				*ptr = points[j];
				ptr++;
				cc++;
			}
		}

		// 					if(cc<childPointCounts[i]) printf("---------");
		// 					if(cc>childPointCounts[i])
		// 					{
		// 						printf("+++++++++");
		// 						printf("depth=%d child# = %d points = %d \n",currentDepth,i,count);
		// 					}
		// 					printf("child# = %d 实际拷贝点数 = %d, childPointCounts = %d  depth = %d\n",i,cc,childPointCounts[i],currentDepth);

		// At this point, we have a list of points that will belong to
		// this child node. We'll want to remove any point with a
		// duplicate 'n' in them...
		//
		// [We won't need to reallocate the list, since it's temporary]

		/*
		// Generate a new bounding volume -- We do this with a touch of
		// trickery...
		//
		// We use a table of offsets. These offsets determine where a
		// node is, relative to it's parent. So, for example, if want to
		// generate the bottom-left-rear (-x, -y, -z) child for a node,
		// we use (-1, -1, -1).
		// 
		// However, since the radius of a child is always half of its
		// parent's, we use a table of 0.5, rather than 1.0.
		// 
		// These values are stored the following table. Note that this
		// won't compile because it assumes Points are structs, but you
		// get the idea.


		Point   boundsOffsetTable[] =
		{
		Point(-0.5, -0.5, -0.5, 0),
		Point(+0.5, -0.5, -0.5, 0),
		Point(-0.5, +0.5, -0.5, 0),
		Point(+0.5, +0.5, -0.5, 0),
		Point(-0.5, -0.5, +0.5, 0),
		Point(+0.5, -0.5, +0.5, 0),
		Point(-0.5, +0.5, +0.5, 0),
		Point(+0.5, +0.5, +0.5, 0)
		};

		// Calculate our offset from the center of the parent's node to
		// the center of the child's node

		Point   offset = boundsOffsetTable[i] * bounds.radius;

		// Create a new Bounds, with the center offset and half the
		// radius

		Bounds  newBounds;
		newBounds.radius = bounds.radius * 0.5;
		newBounds.center = bounds.center + offset;
		*/
		Bounds  newBounds = calcCubicBounds(_child[i]->_points,childPointCounts[i]);
		// Recurse

		_child[i]->build(_child[i]->_points, childPointCounts[i], threshold, maximumDepth,
			newBounds, currentDepth+1);

		// Clean up

	}

	// set all flag bit to 1
	for (int i=0;i<count;i++)
	{
		Point   &p = *(points[i]);
		p.code |= 8;
	}

	return true;
}

// -----------------------------------------------------------------------------
// Determine the [cubic] bounding volume for a set of points
// -----------------------------------------------------------------------------

Bounds Octree::calcCubicBounds(const Point * const * points,
							   const unsigned int count)
{
	// What we'll give to the caller

	Bounds  b;

	// Determine min/max of the given set of points

	Point   min = *points[0];
	Point   max = *points[0];

	for (unsigned int i = 1; i < count; i++)
	{
		const Point &p = *points[i];
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}

	// The radius of the volume (dimensions in each direction)

	Point   radius = max - min;

	// Find the center of this space

	b.center = min + radius * 0.5;

	// We want a CUBIC space. By this, I mean we want a bounding cube, not
	// just a bounding box. We already have the center, we just need a
	// radius that contains the entire volume. To do this, we find the
	// maxumum value of the radius' X/Y/Z components and use that

	b.radius = radius.x;
	if (b.radius < radius.y) b.radius = radius.y;
	if (b.radius < radius.z) b.radius = radius.z;

	// Done

	return b;
}

// -----------------------------------------------------------------------------
// Generic tree traversal
// -----------------------------------------------------------------------------
/*
const bool Octree::traverse(callback proc, void *data) const
{
// Call the callback for this node (if the callback returns false, then
// stop traversing.

if (!proc(*this, data)) return false;

// If I'm a node, recursively traverse my children

if (!_pointCount)
{
for (unsigned int i = 0; i < 8; i++)
{
// We store incomplete trees (i.e. we're not guaranteed
// that a node has all 8 children)

if (!_child[i]) continue;

if (!_child[i]->traverse(proc, data)) return false;
}
}

return true;
}
*/

void Octree::FindMinDistance(const Point &sp, Point &cp, double &rs)
{
	// find min distance in Octree with current r square and source point p
	if (_pointCount)
	{
		if (IsInnerNode()==false)
		{//this is a leaf, compute through its points

			// if the bounding box is outside the range, no need to compute
			if (IsOutsideRadius(sp,rs))
			{
				return;
			}

			// if not, we try to update the closest point and rs
			else
			{
				for (int i=0;i<_pointCount;i++)
				{
					Point &p =  *(_points[i]);
					double newrs = (sp.x-p.x)*(sp.x-p.x)+(sp.y-p.y)*(sp.y-p.y)+(sp.z-p.z)*(sp.z-p.z);
					if (newrs<rs)
					{
						rs = newrs;
						cp = p;
					}
				}

				//Octree::computeCount += _pointCount;
			}
		}

		else
		{// it has children
			for (int i = 0; i < 8; i++)
			{
				// We store incomplete trees (i.e. we're not guaranteed
				// that a node has all 8 children)

				if (!_child[i]) continue;

				_child[i]->FindMinDistance(sp,cp,rs);
			}
		}
	}

}