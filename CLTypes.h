#pragma once
#include "OpenCL.h"

#pragma pack(push,1)

struct cl_RGB32
{
    union {
        cl_uchar CL_ALIGNED(4) s[4];
        struct {
            cl_uchar red;
            cl_uchar green;
            cl_uchar blue;
            cl_uchar alpha;
        };
    };
}; // 4 bytes

struct cl_Material
{
	cl_float3 ambient;
	cl_float3 diffuse;
	cl_float3 specular;
	cl_float shininess;
	cl_float glossiness;
	cl_float reflectivity;
	cl_float transparency;
}; // 64 bytes

struct cl_AAInfo
{
	cl_float inc;
	cl_float div;
	cl_short lvl;
	cl_short dim;
}; // 16 bytes

struct cl_CamInfo {
	cl_double3 bl_ray;
	cl_double3 cam_pos;
	cl_double3 cam_ori;
	cl_double3 cam_fwd;
	cl_double3 cam_rgt;
	cl_double3 cam_up;
	cl_double cam_foc;
}; // 200 bytes

struct cl_RenderInfo
{
	cl_CamInfo cam_info;
	cl_double delta_time;
	cl_double game_time;
	cl_uint3 zone_offset;
	cl_uint rand_int;
	cl_ushort pixels_X;
	cl_ushort pixels_Y;
	cl_ushort half_X;
	cl_ushort half_Y;
	cl_AAInfo aa_info;
}; // 256 bytes

struct cl_RayIntersect
{
	union {
		struct {
			cl_float3 point;
			cl_float3 normal;
		};
		struct {
			cl_float pnt[3];
			cl_float depth;
			cl_float nrm[3];
			cl_uint index;
		};
	};
}; // 32 bytes

struct cl_InstanceData {
    cl_float4 color;
    cl_float2 offset;
    cl_float scale;
    cl_float depth;
}; // 32 bytes

struct cl_SpriteData {
    cl_float4 color;
    cl_float2 offset;
    cl_float scale;
    cl_float rotation;
    cl_float depth;
}; // 36 bytes

struct cl_AnimSpriteData {
    cl_float4 color;
    cl_float2 offset;
    cl_float scale;
    cl_float rotation;
    cl_float depth;
    cl_uint frame;
}; // 40 bytes

struct cl_Star {
    union {
        cl_double3 position;
        struct {
            cl_double x_pos;
            cl_double y_pos;
            cl_double z_pos;
            cl_ushort age;
            cl_uchar oxygen;
            cl_uchar carbon;
            cl_uchar hydrog;
            cl_uchar helium;
            cl_uchar metal;
            cl_uchar type;
        };
    };
	cl_float mass;
	cl_float luminosity;
	cl_float radius;
	cl_float density;
	cl_float volume;
    cl_float rot_time;
	cl_float temp;
	cl_RGB32 color;
}; // 64 bytes

struct cl_Planet {
    union {
        cl_double3 position;
        struct {
            cl_double x_pos;
            cl_double y_pos;
            cl_double z_pos;
            cl_ushort age;
            cl_uchar moons;
            cl_uchar species;
            cl_uchar hostility;
            cl_uchar habitability;
            cl_uchar life_level;
            cl_uchar type;
        };
    };
    cl_float2 orbit;
	cl_float mass;
	cl_float radius;
	cl_float density;
	cl_float gravity;
	cl_float area;
	cl_float temp;
	cl_float water_frac;
    cl_float rot_time;
    cl_float orbit_time;
	cl_float orbit_rad;
	cl_float eccentricity;
	cl_float velocity;
	cl_float atmos_radi;
	cl_RGB32 atmos_spec;
}; // 96 bytes

struct cl_Moon {
    union {
        cl_double3 position;
        struct {
            cl_double x_pos;
            cl_double y_pos;
            cl_double z_pos;
            cl_ushort age;
            cl_uchar function;
            cl_uchar species;
            cl_uchar hostility;
            cl_uchar habitability;
            cl_uchar life_level;
            cl_uchar type;
        };
    };
    cl_float2 orbit;
	cl_float mass;
	cl_float radius;
	cl_float density;
	cl_float gravity;
	cl_float area;
	cl_float temp;
	cl_float water_frac;
    cl_float rot_time;
    cl_float orbit_time;
	cl_float orbit_rad;
	cl_float eccentricity;
    cl_float velocity;
	cl_float atmos_radi;
	cl_RGB32 atmos_spec;
}; // 96 bytes

struct cl_SolarSystem {
	cl_double2 plane;
	cl_ulong index;
	cl_uint conflict;
	cl_uint economy;
	cl_uint security;
	cl_uint civ_count;
	cl_uint moons;
	cl_uint planets;
	cl_uint stations;
	cl_uint artifacts;
	cl_uint anomalies;
	cl_uint secrets;
}; // 64 bytes

/*struct cl_Galaxy {
	cl_double3 position;
	cl_uint stars;
	cl_float mass;
	cl_float luminosity;
	cl_float radius;
	cl_RGB32 color;
	cl_uint ratios;
	cl_uchar type;
	cl_uchar age;
	cl_uchar oxygen;
	cl_uchar carbon;
	cl_uchar hydrog;
	cl_uchar helium;
	cl_uchar metal;
	cl_uchar dust;
}; // 64 bytes*/

#pragma pack(pop)
