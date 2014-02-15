/*
    File: paintarm.h

    Authors: Wesley Tang
            Department of Computer Science
            Texas A&M University
		

*/

#pragma once
#include "global_const.h"
#include "matrix.h"

class PaintArm {
	private:
		Matrix* matT_Z_rot(double theta_in_deg, double offset_x, double offset_y, double offset_z);
		Matrix* matT_base_to_joint_n(int n);
	protected:
		std::vector<Matrix*> _T_Matrices;
		std::vector<Matrix*> base_to_n;
		int num_of_joints;
	public:
		PaintArm();
		~PaintArm();
		Matrix* get_T_Matrix(int start_index, int end_index);
		void rotate(int joint_index, double rotation_in_deg);
		
};



