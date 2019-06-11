#pragma once
#include "MathExt.h"
#include "Colors.h"
#include "Vec3.h"

class Camera {
public:
    DVec3 bl_ray;
    DVec3 position;
    DVec3 orientation;
    DVec3 forward;
    DVec3 right;
    DVec3 up;
    float foclen;
    //float aptrad;
	float sensitivity;
public:
	Camera() {
		position = DV3_V0;
		orientation = DV3_V0;
		forward = DV3_Z1;
		right = DV3_X1;
		up = DV3_Y1;
		foclen = 600.0f;
		//aptrad = 0.0f;
	}
	Camera(DVec3 pos, DVec3 ori, DVec3 fwd, DVec3 rgt, DVec3 upv, float fl) :
		position(pos),
		orientation(ori),
		forward(fwd),
		right(rgt),
		up(upv),
		foclen(fl)
		//aptrad(ar)
	{}
	cl_CamInfo GetInfo()
	{
	    cl_CamInfo cam_info;
	    cam_info.bl_ray = bl_ray.vector;
	    cam_info.cam_pos = position.vector;
	    cam_info.cam_ori = orientation.vector;
	    cam_info.cam_fwd = forward.vector;
	    cam_info.cam_rgt = right.vector;
	    cam_info.cam_up = up.vector;
	    cam_info.cam_foc = foclen;
	    //cam_info.cam_apt = aptrad;
	    return cam_info;
	}
	DVec3 PointRelCam(const DVec3& point) const
	{
		return point.VectSub(position).VectRot(orientation);
	}
	void UpdateDirection()
	{
		NormAngle(orientation.x);
		NormAngle(orientation.y);
		NormAngle(orientation.z);
		DVec3 ori = orientation.VectNeg();
		forward = DV3_Z1.VectRev(ori);
		right = DV3_X1.VectRev(ori);
		up = DV3_Y1.VectRev(ori);
	}
};
