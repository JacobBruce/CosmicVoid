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
	float sensitivity;

	Camera()
	{
		position = DV3_V0;
		orientation = DV3_V0;
		forward = DV3_Z1;
		right = DV3_X1;
		up = DV3_Y1;
		foclen = 800.0f;
		sensitivity = 0.00005f;
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

	glm::vec3 Forward() const
	{
	    return glm::vec3(forward.x, forward.y, forward.z);
	}

	glm::vec3 Right() const
	{
	    return glm::vec3(right.x, right.y, right.z);
	}

	glm::vec3 Up() const
	{
	    return glm::vec3(up.x, up.y, up.z);
	}
};

class GLcam {
protected:
    glm::mat4 sView;
    const glm::vec3 sForward = glm::vec3(0.0f,0.0f,1.0f);
    const glm::vec3 sRight = glm::vec3(1.0f,0.0f,0.0f);
    const glm::vec3 sUp = glm::vec3(0.0f,1.0f,0.0f);
public:
    glm::vec3 position;
    glm::mat4 view;
    float fov;
	float sensitivity;

    GLcam()
	{
		position = glm::vec3(0.0f,0.0f,0.0f);
		sView = glm::lookAt(position, sForward, sUp);
		view = sView;
		sensitivity = 0.00005f;
		fov = 60.0f;
	}

	void Rotate(const GLfloat angle_x, const GLfloat angle_y, const GLfloat angle_z)
	{
        view = glm::rotate(glm::rotate(glm::rotate(sView, angle_x, sRight), angle_z, sForward), angle_y, sUp);
	}

	void CalcFOV(const float foc_len, const float width_half)
	{
	    glm::vec3 screenLeft((sForward * foc_len) - (sRight * width_half));
	    glm::vec3 screenRight((sForward * foc_len) + (sRight * width_half));
        fov = std::acos(glm::dot(screenLeft, screenRight) / (glm::length(screenLeft) * glm::length(screenRight)));
	}
};
