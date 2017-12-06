#pragma comment(lib,"nvModel.lib")


#include <time.h>

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Vector.h"
#include "octree.h"
#include "nvModel.h"
#include "ICP.h"
#include "IcpRegistration.h"

using namespace std;

IcpRegistration::IcpRegistration(void)
{
	modelFile1 = "./1.obj";
	modelFile2 = "./2.obj";

	total_point1 = 0;
	total_point2 = 0;

	IterPass = 0;
	ctrl_p =  200;


}

IcpRegistration::~IcpRegistration(void)
{
}

void IcpRegistration::LoadObj1(char* file)
{
	nv::Model *model1 = new nv::Model;

	if (!model1->loadModelFromFile( modelFile1)) 
	{
		delete model1;
		model1 = NULL;
		printf( "Error: Failed to load model: %s\n", modelFile1);
		exit(-1);
	}

	total_point1 = model1->getPositionCount();

	for (int i=0;i<total_point1;i++)
	{
		pData1[i].x = model1->getPositions()[3*i+0];
		pData1[i].y = model1->getPositions()[3*i+1];
		pData1[i].z = model1->getPositions()[3*i+2];
	}
}

void IcpRegistration::LoadObj2(char* file)
{
	nv::Model *model2 = new nv::Model;
	
	if (!model2->loadModelFromFile( modelFile2)) 
	{
		delete model2;
		model2 = NULL;
		printf( "Error: Failed to load model: %s\n", modelFile2);
		exit(-1);
	}

	total_point2 = model2->getPositionCount();
	
	for (int i=0;i<total_point2;i++)
	{
		pData2[i].x = model2->getPositions()[3*i+0];
		pData2[i].y = model2->getPositions()[3*i+1];
		pData2[i].z = model2->getPositions()[3*i+2];
		pData2[i].code = 0;
	}
}

void IcpRegistration::Reset(void)
{
	IterPass = 0;
	error_decrease = 0;
	icp.ResetParameter();
	

	srand(time(NULL));

	pData1.resize(36000);
	pData2.resize(36000);

	LoadObj1(modelFile1);//load data from 1.obj
	LoadObj2(modelFile2);//load data from 2.obj
	
	//////////////////////////////////////////////////////////////////////////

	// build octree for model2
	dataPointer = new Point*[total_point2];
	
	for (int i=0;i<total_point2;i++)
	{
		dataPointer[i] = (Point*)&(pData2[i]);
	}

	// initial bounding box
	Bounds bb = oc.calcCubicBounds(dataPointer,total_point2);

	printf("building octree...\n");
	oc.build(dataPointer,total_point2,100,10,bb);



	// random select control points from pData1
	vector<int> tmp;
	int p;
	while (pControl.size()<ctrl_p)
	{
RAND_STAGE:		
		p = rand()%total_point1;
		for (int i=0;i<tmp.size();i++)
		{//check if it exist?
			if (p==tmp.at(i))
			{
				goto RAND_STAGE;
			}
		}
		tmp.push_back(p);
		pControl.push_back(pData1[p]);

	}

	icp.EvaluateCurrentClosestPointOCtree(pControl, pData2, oc, x);	// x is map table
	icp.EvaluateRT(pControl, x, icp.icp_r, icp.icp_t);		// compute r ,t
	err = icp.EvaluateError(pControl, x, icp.icp_r, icp.icp_t);	// compute error
	for (int i = 0; i < 3; i++) one_step_r[i] = icp.icp_r[i];	
	one_step_t = icp.icp_t;
}

void IcpRegistration::NextStep(void)
{
	IterPass++;
	icp.OneStepMove(pControl, one_step_r, one_step_t);		// transform A to A',  A is changed!!!
	icp.OneStepMove(pData1, one_step_r, one_step_t);			// pData1 is changed!!!

	icp.EvaluateCurrentClosestPointOCtree(pControl, pData2, oc, x);	// map A' to B in x
	icp.EvaluateRT(pControl, x, one_step_r, one_step_t);		// compute one_step r and t
	icp.UpdateRT(icp.icp_r, icp.icp_t, one_step_r, one_step_t);			// accum one_step_r to already got icp_r 
	double new_err = icp.EvaluateError(pControl, x, one_step_r, one_step_t);	// current step error
	 error_decrease = (std::abs(new_err*1000000.0 - err*1000000.0) / (err*1000000.0))*100.0;
	err = new_err;
}

void IcpRegistration::GetAverageXZ(float* x, float* z)
{
	float sumX = 0, sumZ = 0;
	for(int i = 0;i < this->total_point1;i++)
	{
		sumX += this->pData1[i].x;
		sumZ += this->pData1[i].z;
	}
	*x = sumX/total_point1;
	*z = sumZ/total_point1;

}
