#pragma once

#include <vector>

#include "ICP.h"

using namespace std;

class IcpRegistration
{
public:
	IcpRegistration(void);
	~IcpRegistration(void);
	void LoadObj1(char* file);
	void LoadObj2(char* file);

	//model and the default model file

	char* modelFile1;
	char* modelFile2;
	//Vector3D *pData1,*pData2;
	vector<Vector3D> pData1,pControl;
	vector<Point>	pData2;

	int total_point1,total_point2;
	CICP icp;

	Octree oc;		// octree
	Point **dataPointer;

	double err;
	vector<::Vector3D> x;
	Vector3D one_step_r[3];
	Vector3D one_step_t;
	int IterPass;
	int ctrl_p;
	double error_decrease;

	void Reset(void);
	void NextStep(void);
	void GetAverageXZ(float* x, float* z);
};
