#include "ICP.h"
#include "Vector.h"
#include "./TNT/jama_eig.h"

CICP::CICP(void)
{
	icp_r[0] = Vector3D(0.0f,0.0f,0.0f);
	icp_r[1] = Vector3D(0.0f,0.0f,0.0f);
	icp_r[2] = Vector3D(0.0f,0.0f,0.0f);
	icp_t = Vector3D(0.0f,0.0f,0.0f);

	ptIndex=new vector<int>;

}

CICP::~CICP(void)
{
}


void CICP::ResetParameter()
{
	icp_r[0] = Vector3D(0.0f,0.0f,0.0f);
	icp_r[1] = Vector3D(0.0f,0.0f,0.0f);
	icp_r[2] = Vector3D(0.0f,0.0f,0.0f);
	icp_t = Vector3D(0.0f,0.0f,0.0f);
}


void CICP::IterativeClosestPoint() {  // Need add precheck here
	std::vector<::Vector3D> x;
	EvaluateCurrentClosestPoint(pointlist_A, pointlist_B, x);
	EvaluateRT(pointlist_A, x, icp_r, icp_t);
	double err = EvaluateError(pointlist_A, x, icp_r, icp_t);
	::Vector3D one_step_r[3];
	for (int i = 0; i < 3; i++) one_step_r[i] = icp_r[i];
	::Vector3D one_step_t = icp_t;
	while (true) {
		OneStepMove(pointlist_A, one_step_r, one_step_t);		// transform A to A'
		EvaluateCurrentClosestPoint(pointlist_A, pointlist_B, x);	// map A' to B in x
		EvaluateRT(pointlist_A, x, one_step_r, one_step_t);		// compute one_step r and t
		UpdateRT(icp_r, icp_t, one_step_r, one_step_t);			// accum one_step_r to already got icp_r 
		double new_err = EvaluateError(pointlist_A, x, one_step_r, one_step_t);	// current step error
		if(err==0)
			break;
		double delta = std::abs(new_err - err) / err;
		//cout<<"error= "<<new_err<<" "<<err<<" "<<delta<<endl;
		if (delta < 0.01) break;  // Iterative termination threshold
		err = new_err;
	}
}

// brute force search
void CICP::EvaluateCurrentClosestPoint(std::vector<::Vector3D>& a,
						 std::vector<::Vector3D>& b,
						 std::vector<::Vector3D>& x)
{
	ptIndex->clear();
		 int num_a = a.size();
		 int num_b = b.size();
		 x.clear();
		 x.resize(num_a);
		 for (int i = 0; i < num_a; ++i) {
			 double min_length = Length(a[i] - b[0]);
			 int map_to = 0;
			 for (int j = 1; j < num_b; ++j) {
				 double current_length = Length(a[i] - b[j]);
				 if (current_length < min_length) {
					 min_length = current_length;
					 map_to = j;
				 }
			 }
			 x[i] = b[map_to];
			 //printf("find the %d's Closest point is %d\n",i,map_to);
			 ptIndex->push_back(map_to);
		 }
}

// Octree search
// dataPointer is copied from pData2
// oc.build(dataPointer,total_point2,100,10,bb);
// pControl.push_back(pData1[p]);
// icp.EvaluateCurrentClosestPointOCtree(pControl, pData2, oc, x);	// x is map table
void CICP::EvaluateCurrentClosestPointOCtree(std::vector<::Vector3D>& a,
										std::vector<Point>& b,
									   Octree &oc,
									   std::vector<::Vector3D>& x // out
									   )
{
	int num_a = a.size();
	x.clear();
	x.resize(num_a);

	for (int i = 0; i < num_a; ++i) 
	{

		Point ca = b[0];
		double rs = (a[i].x-ca.x)*(a[i].x-ca.x)+(a[i].y-ca.y)*(a[i].y-ca.y)+(a[i].z-ca.z)*(a[i].z-ca.z);

		oc.FindMinDistance(Point(a[i].x,a[i].y,a[i].z),ca,rs);

		x[i].x = ca.x;
		x[i].y = ca.y;
		x[i].z = ca.z;
		printf("find (%f,%f,%f) Closest point is (%f,%f,%f)\n",a[i].x,a[i].y,a[i].z,x[i].x,x[i].y,x[i].z);
	}
}


void CICP::EvaluateRT(std::vector<::Vector3D>& a,
				std::vector<::Vector3D>& x,
				::Vector3D* r,
				::Vector3D& t) {
		::Vector3D center_a(0.0, 0.0, 0.0);
		int num = a.size();
		for (int i = 0; i < num; ++i) {
			center_a += a[i];
		}
		center_a /= (double)num;
		::Vector3D center_x(0.0, 0.0, 0.0);
		for (int i = 0; i < num; ++i) {
			center_x += x[i];
		}
		center_x /= (double)num;
		double cross_covariance_matrix[3][3];
		std::memset(cross_covariance_matrix, 0, sizeof(cross_covariance_matrix));
		for (int i = 0; i < num; ++i) {
			::Vector3D p = a[i] - center_a;
			::Vector3D q = x[i] - center_x;
			for (int j = 0; j < 3; ++j)
				for (int k = 0; k < 3; ++k)
					cross_covariance_matrix[j][k] += p[j] * q[k];
		}
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				cross_covariance_matrix[j][k] /= (double)num;
		double matrix_A[3][3];
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				matrix_A[j][k] = cross_covariance_matrix[j][k] -
				cross_covariance_matrix[k][j];
		double tr_ccm = cross_covariance_matrix[0][0] +
			cross_covariance_matrix[1][1] +
			cross_covariance_matrix[2][2];
		double matrix_Q[4][4];
		matrix_Q[0][0] = tr_ccm;
		matrix_Q[0][1] = matrix_A[1][2];
		matrix_Q[0][2] = matrix_A[2][0];
		matrix_Q[0][3] = matrix_A[0][1];
		matrix_Q[1][0] = matrix_A[1][2];
		matrix_Q[2][0] = matrix_A[2][0];
		matrix_Q[3][0] = matrix_A[0][1];
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				matrix_Q[j + 1][k + 1] = cross_covariance_matrix[j][k] +
				cross_covariance_matrix[k][j];
		for (int j = 0; j < 3; j++) {
			matrix_Q[j + 1][j + 1] -= tr_ccm;
		}
		double e[4];  // eigenvector
		EvaluateEigenvector(matrix_Q, e);
		r[0][0] = e[0] * e[0] + e[1] * e[1] - e[2] * e[2] - e[3] * e[3];
		r[1][1] = e[0] * e[0] - e[1] * e[1] + e[2] * e[2] - e[3] * e[3];
		r[2][2] = e[0] * e[0] - e[1] * e[1] - e[2] * e[2] + e[3] * e[3];
		r[0][1] = (e[1] * e[2] - e[0] * e[3]) * 2;
		r[1][0] = (e[1] * e[2] + e[0] * e[3]) * 2;
		r[0][2] = (e[1] * e[3] + e[0] * e[2]) * 2;
		r[2][0] = (e[1] * e[3] - e[0] * e[2]) * 2;
		r[1][2] = (e[2] * e[3] - e[0] * e[1]) * 2;
		r[2][1] = (e[2] * e[3] + e[0] * e[1]) * 2;
		//for (int i = 0; i < 3; ++i)
		//  for (int j = 0; j < 3; ++j) {
		//    if (abs(r[i][j]) < 0.05) r[i][j] = 0;
		//    else if (abs(r[i][j] - 1.0) < 0.05) r[i][j] = 1;
		//  }
		::Vector3D v(r[0] * center_a, r[1] * center_a, r[2] * center_a);
		t = center_x - v;
}


void CICP::EvaluateRT(::Vector3D center_a, ::Vector3D center_b,
				double matrix_q[][4],
				::Vector3D* r, ::Vector3D* t) 
{
		double e[4];  // eigenvector
		EvaluateEigenvector(matrix_q, e);
		r[0][0] = e[0] * e[0] + e[1] * e[1] - e[2] * e[2] - e[3] * e[3];
		r[1][1] = e[0] * e[0] - e[1] * e[1] + e[2] * e[2] - e[3] * e[3];
		r[2][2] = e[0] * e[0] - e[1] * e[1] - e[2] * e[2] + e[3] * e[3];
		r[0][1] = (e[1] * e[2] - e[0] * e[3]) * 2;
		r[1][0] = (e[1] * e[2] + e[0] * e[3]) * 2;
		r[0][2] = (e[1] * e[3] + e[0] * e[2]) * 2;
		r[2][0] = (e[1] * e[3] - e[0] * e[2]) * 2;
		r[1][2] = (e[2] * e[3] - e[0] * e[1]) * 2;
		r[2][1] = (e[2] * e[3] + e[0] * e[1]) * 2;
		::Vector3D v(r[0] * center_a, r[1] * center_a, r[2] * center_a);
		*t = center_b - v;
}


void CICP::EvaluateEigenvector(double matrix[][4], double* eigenvector) 
{
	Array2D<double> W(4,4);
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			W[i][j] = matrix[i][j];

	JAMA::Eigenvalue<double> CC(W);
	Array1D<double> Eingivalue;
	CC.getRealEigenvalues(Eingivalue);
	double max_eig = Eingivalue[0];
	int map_i = 0;
	for (int i = 1; i < 4; ++i) {
		if (Eingivalue[i] > max_eig) {
			max_eig = Eingivalue[i];
			map_i = i;
		}
	}

	Array2D<double> Eingivector;
	CC.getV(Eingivector);
	for (int i = 0 ; i < 4 ; ++i)
		eigenvector[i] = Eingivector[i][map_i];
}


double CICP::EvaluateError(std::vector<::Vector3D>& a,
				     std::vector<::Vector3D>& x,
				     ::Vector3D* r,
				     ::Vector3D& t)
{
	     double ret = 0;
	     int num = a.size();
	     for (int i = 0; i < num; ++i) {
		     ::Vector3D v(r[0] * a[i], r[1] * a[i], r[2] * a[i]);
		     v *= -1;
		     v += x[i];
		     v -= t;
		     ret += v.GetLength2();
	     }
	     ret /= (double)num;
	     return ret;
}


void CICP::OneStepMove(std::vector<::Vector3D>& a,
				 ::Vector3D* r,
				 ::Vector3D& t)
{
	 int num = a.size();
	 for (int i = 0; i < num; ++i)
	 {
		 ::Vector3D v(r[0] * a[i], r[1] * a[i], r[2] * a[i]);
		 a[i] = v + t;
	 }
}

void CICP::UpdateRT(::Vector3D* r,
			      ::Vector3D& t,
			      ::Vector3D* one_step_r,
			      ::Vector3D& one_step_t)
{
	      ::Vector3D pre_r[3];
	      for (int i = 0; i < 3; ++i) pre_r[i] = r[i];
	      for (int j = 0; j < 3; ++j)
		      for (int k = 0; k < 3; ++k)
			      r[j][k] = one_step_r[j][0] * pre_r[0][k] +
			      one_step_r[j][1] * pre_r[1][k] +
			      one_step_r[j][2] * pre_r[2][k];
	      ::Vector3D pre_t = t;
	      t[0] = one_step_r[0] * pre_t;
	      t[1] = one_step_r[1] * pre_t;
	      t[2] = one_step_r[2] * pre_t;
	      t += one_step_t;
}