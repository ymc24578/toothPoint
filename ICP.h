#pragma once

#include <vector>
#include "Vector.h"

#include "octree.h"

using namespace std;
class CICP
{
public:
	CICP(void);
	~CICP(void);

	void ResetParameter();
	void IterativeClosestPoint();

	void OneStepMove(std::vector<::Vector3D>& a,
		::Vector3D* r,
		::Vector3D& t);

	void EvaluateCurrentClosestPoint(std::vector<::Vector3D>& a,
		std::vector<::Vector3D>& b,
		std::vector<::Vector3D>& x);

	void EvaluateCurrentClosestPointOCtree(std::vector<::Vector3D>& a,
		std::vector<Point>& b,
		Octree &oc,
		std::vector<::Vector3D>& x);


	// Using TNT lib for eigen calculating
	void EvaluateEigenvector(double matrix[][4], double* eigenvector);

	void EvaluateRT(std::vector<::Vector3D>& a,
		std::vector<::Vector3D>& x,
		::Vector3D* r,
		::Vector3D& t);

	void EvaluateRT(::Vector3D center_a, ::Vector3D center_b,
		double matrix_q[][4],
		::Vector3D* r, ::Vector3D* t);

	void UpdateRT(::Vector3D* r,
		::Vector3D& t,
		::Vector3D* one_step_r,
		::Vector3D& one_step_t);

	double EvaluateError(std::vector<::Vector3D>& a,
		std::vector<::Vector3D>& x,
		::Vector3D* r,
		::Vector3D& t);


	std::vector<::Vector3D> pointlist_A;
	std::vector<::Vector3D> pointlist_B;
	::Vector3D icp_r[3];
	::Vector3D icp_t;
	vector<int> *ptIndex;

};
