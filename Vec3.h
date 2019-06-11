#pragma once
#include "MathExt.h"
#include "Resource.h"

class Vec3 {
public:
	union {
		cl_float3 vector;
		struct {
			float x;
			float y;
			float z;
			float w;
		};
	};
	Vec3() :
        x(0.0f),
        y(0.0f),
        z(0.0f)
	{}
	Vec3(float X, float Y, float Z) :
		x(X),
		y(Y),
		z(Z)
	{}
	inline void set(Vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline void set(const float X, const float Y, const float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	inline void reset()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	inline bool isZero() const
	{
		if (x == 0.0f && y == 0.0f && z == 0.0f) {
			return true;
		} else {
			return false;
		}
	}
	static Vec3 VectRnd(uint64_t seed)
	{
	    Vec3 result;
	    uint64_t rnd1 = RandomLong(seed);
	    uint64_t rnd2 = RandomLong(rnd1);
	    uint64_t rnd3 = RandomLong(rnd2);
	    result.x = RandDblFromLng(rnd1);
	    result.y = RandDblFromLng(rnd2);
	    result.z = RandDblFromLng(rnd3);
	    return result;
	}
	inline float VectMag() const
	{
		return sqrt((x*x) + (y*y) + (z*z));
	}
	inline float VectSqrd(const Vec3& v) const
	{
		return pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2);
	}
	inline float VectDist(const Vec3& v) const
	{
		return sqrt(pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2));
	}
	inline float VectDot(const Vec3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	inline float VectAngle(const Vec3& v) const
	{
		return acos(v.VectDot(*this));
	}
	inline Vec3 VectRefl(Vec3& norm) const
	{
		return ((norm * -2) * VectDot(norm)).VectAdd(*this);
	}
	inline Vec3 VectNorm() const
	{
		float mag = VectMag();
		return Vec3(x/mag, y/mag, z/mag);
	}
	inline Vec3 VectNeg() const
	{
		return Vec3(-x, -y, -z);
	}
	inline Vec3 VectAvg(const Vec3& v) const
	{
		return Vec3((x+v.x) / 2.0f, (y+v.y) / 2.0f, (z+v.z) / 2.0f);
	}
	inline Vec3 VectBlend(const Vec3& v, const float alpha) const
	{
		float invAlpha = 1.0f - alpha;
		return Vec3(v.x*alpha + x*invAlpha, v.y*alpha + y*invAlpha, v.z*alpha + z*invAlpha);
	}
	inline Vec3 VectCross(const Vec3& v) const
	{
		return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	inline Vec3 VectMult(const Vec3& v) const
	{
		return Vec3(x * v.x, y * v.y, z * v.z);
	}
	inline Vec3 VectDiv(const Vec3& v) const
	{
		return Vec3(x / v.x, y / v.y, z / v.z);
	}
	inline Vec3 VectAdd(const Vec3& v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	inline Vec3 VectSub(const Vec3& v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	inline Vec3 VectMax(const float& m)
	{
		return Vec3(std::max(x,m), std::max(y,m), std::max(z,m));
	}
	inline Vec3 VectMin(const float& m)
	{
		return Vec3(std::min(x,m), std::min(y,m), std::min(z,m));
	}
	inline Vec3 VectRotX(const float& angle) const
	{
		return Vec3(x, (z*sin(angle)) + (y*cos(angle)), (z*cos(angle)) - (y*sin(angle)));
	}
	inline Vec3 VectRotY(const float& angle) const
	{
		return Vec3((z*sin(angle)) + (x*cos(angle)), y, (z*cos(angle)) - (x*sin(angle)));
	}
	inline Vec3 VectRotZ(const float& angle) const
	{
		return Vec3((x*cos(angle)) - (y*sin(angle)), (x*sin(angle)) + (y*cos(angle)), z);
	}
	inline Vec3 VectRot(const Vec3& rot) const
	{
		return VectRotY(rot.y).VectRotZ(rot.z).VectRotX(rot.x);
	}
	inline Vec3 VectRev(const Vec3& rot) const
	{
		return VectRotX(rot.x).VectRotZ(rot.z).VectRotY(rot.y);
	}
	inline Vec3 operator*(const float& scalar) const
	{
		return Vec3(x*scalar, y*scalar, z*scalar);
	}
	inline Vec3 operator/(const float& scalar) const
	{
		return Vec3(x/scalar, y/scalar, z/scalar);
	}
	inline Vec3 operator+(const float& scalar) const
	{
		return Vec3(x+scalar, y+scalar, z+scalar);
	}
	inline Vec3 operator-(const float& scalar) const
	{
		return Vec3(x-scalar, y-scalar, z-scalar);
	}
	inline bool operator==(const Vec3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	inline bool operator!=(const Vec3 &rhs) const
	{
		return !(*this == rhs);
	}
	inline void operator+=(const Vec3 &rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
	}
	inline void operator-=(const Vec3 &rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
	}
	inline void operator*=(const Vec3 &rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
	}
	inline void operator/=(const Vec3 &rhs)
	{
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
	}
};

class DVec3 {
public:
	union {
		cl_double3 vector;
		struct {
			double x;
			double y;
			double z;
			double w;
		};
	};
	DVec3() :
        x(0.0),
        y(0.0),
        z(0.0)
	{}
	DVec3(const cl_double3& v) :
        x(v.s[0]),
        y(v.s[1]),
        z(v.s[2])
	{}
	DVec3(double X, double Y, double Z) :
		x(X),
		y(Y),
		z(Z)
	{}
	inline void set(const DVec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline void set(const cl_double3& v)
	{
		vector = v;
	}
	inline void set(const double X, const double Y, const double Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	inline void reset()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	inline bool isZero() const
	{
		if (x == 0.0 && y == 0.0 && z == 0.0) {
			return true;
		} else {
			return false;
		}
	}
	static DVec3 VectRnd(uint64_t seed)
	{
	    DVec3 result;
	    uint64_t rnd1 = RandomLong(seed);
	    uint64_t rnd2 = RandomLong(rnd1);
	    uint64_t rnd3 = RandomLong(rnd2);
	    result.x = RandDblFromLng(rnd1);
	    result.y = RandDblFromLng(rnd2);
	    result.z = RandDblFromLng(rnd3);
	    return result;
	}
	inline double VectMag() const
	{
		return sqrt((x*x) + (y*y) + (z*z));
	}
	inline double VectSqrd(const DVec3& v) const
	{
		return pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2);
	}
	inline double VectDist(const DVec3& v) const
	{
		return sqrt(pow(v.x-x, 2) + pow(v.y-y, 2) + pow(v.z-z, 2));
	}
	inline double VectDot(const DVec3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
	inline double VectAngle(const DVec3& v) const
	{
		return acos(v.VectDot(*this));
	}
	inline DVec3 VectRefl(DVec3& norm) const
	{
		return ((norm * -2) * VectDot(norm)).VectAdd(*this);
	}
	inline DVec3 VectNorm() const
	{
		double mag = VectMag();
		return DVec3(x/mag, y/mag, z/mag);
	}
	inline DVec3 VectNeg() const
	{
		return DVec3(-x, -y, -z);
	}
	inline DVec3 VectAvg(const DVec3& v) const
	{
		return DVec3((x+v.x) / 2.0, (y+v.y) / 2.0, (z+v.z) / 2.0);
	}
	inline DVec3 VectBlend(const DVec3& v, const double alpha) const
	{
		double invAlpha = 1.0 - alpha;
		return DVec3(v.x*alpha + x*invAlpha, v.y*alpha + y*invAlpha, v.z*alpha + z*invAlpha);
	}
	inline DVec3 VectCross(const DVec3& v) const
	{
		return DVec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	inline DVec3 VectMult(const DVec3& v) const
	{
		return DVec3(x * v.x, y * v.y, z * v.z);
	}
	inline DVec3 VectDiv(const DVec3& v) const
	{
		return DVec3(x / v.x, y / v.y, z / v.z);
	}
	inline DVec3 VectAdd(const DVec3& v) const
	{
		return DVec3(x + v.x, y + v.y, z + v.z);
	}
	inline DVec3 VectSub(const DVec3& v) const
	{
		return DVec3(x - v.x, y - v.y, z - v.z);
	}
	inline DVec3 VectMax(const double& m)
	{
		return DVec3(std::max(x,m), std::max(y,m), std::max(z,m));
	}
	inline DVec3 VectMin(const double& m)
	{
		return DVec3(std::min(x,m), std::min(y,m), std::min(z,m));
	}
	inline DVec3 VectRotX(const double& angle) const
	{
		return DVec3(x, (z*sin(angle)) + (y*cos(angle)), (z*cos(angle)) - (y*sin(angle)));
	}
	inline DVec3 VectRotY(const double& angle) const
	{
		return DVec3((z*sin(angle)) + (x*cos(angle)), y, (z*cos(angle)) - (x*sin(angle)));
	}
	inline DVec3 VectRotZ(const double& angle) const
	{
		return DVec3((x*cos(angle)) - (y*sin(angle)), (x*sin(angle)) + (y*cos(angle)), z);
	}
	inline DVec3 VectRot(const DVec3& rot) const
	{
		return VectRotY(rot.y).VectRotZ(rot.z).VectRotX(rot.x);
	}
	inline DVec3 VectRev(const DVec3& rot) const
	{
		return VectRotX(rot.x).VectRotZ(rot.z).VectRotY(rot.y);
	}
	inline DVec3 operator*(const double& scalar) const
	{
		return DVec3(x*scalar, y*scalar, z*scalar);
	}
	inline DVec3 operator/(const double& scalar) const
	{
		return DVec3(x/scalar, y/scalar, z/scalar);
	}
	inline DVec3 operator+(const double& scalar) const
	{
		return DVec3(x+scalar, y+scalar, z+scalar);
	}
	inline DVec3 operator-(const double& scalar) const
	{
		return DVec3(x-scalar, y-scalar, z-scalar);
	}
	inline bool operator==(const DVec3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	inline bool operator!=(const DVec3 &rhs) const
	{
		return !(*this == rhs);
	}
	inline void operator+=(const DVec3 &rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
	}
	inline void operator-=(const DVec3 &rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
	}
	inline void operator*=(const DVec3 &rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
	}
	inline void operator/=(const DVec3 &rhs)
	{
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
	}
};

static Vec3 V3_X1 = Vec3( 1.0f, 0.0f, 0.0f );
static Vec3 V3_Y1 = Vec3( 0.0f, 1.0f, 0.0f );
static Vec3 V3_Z1 = Vec3( 0.0f, 0.0f, 1.0f );
static Vec3 V3_V0 = Vec3( 0.0f, 0.0f, 0.0f );
static Vec3 V3_V1 = Vec3( 1.0f, 1.0f, 1.0f );

static DVec3 DV3_X1 = DVec3( 1.0, 0.0, 0.0 );
static DVec3 DV3_Y1 = DVec3( 0.0, 1.0, 0.0 );
static DVec3 DV3_Z1 = DVec3( 0.0, 0.0, 1.0 );
static DVec3 DV3_V0 = DVec3( 0.0, 0.0, 0.0 );
static DVec3 DV3_V1 = DVec3( 1.0, 1.0, 1.0 );
