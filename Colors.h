#pragma once
#include "Vec3.h"

struct RGB24
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;

    RGB24() : blue(0), green(0), red(0) {}

    RGB24(unsigned char r, unsigned char g, unsigned char b) :
        blue(b), green(g), red(r) {}
};

struct RGB32
{
    union {
        cl_RGB32 rgba;
        #pragma pack(push,1)
        struct {
            unsigned char blue;
            unsigned char green;
            unsigned char red;
            unsigned char alpha;
        };
        #pragma pack(pop)
    };

    RGB32() : blue(0), green(0), red(0), alpha(255) {}

    RGB32(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        blue(b), green(g), red(r), alpha(a) {}
};

inline Vec3 RGB24toVec3(const RGB24 src)
{
	return Vec3(src.red/255.0f, src.green/255.0f, src.blue/255.0f);
}

inline Vec3 RGBNtoVec3(const RGB24 src)
{
	return (RGB24toVec3(src) * 2) - 1;
}

inline RGB32 Vec3ToRGB32(const Vec3 v)
{
	return RGB32(v.x, v.y, v.z, 255);
}

inline RGB32 Vec3ToColor(const Vec3 v)
{
	return RGB32(v.x*255.0f, v.y*255.0f, v.z*255.0f, 255);
}

inline float Brightness(const RGB32& c)
{
	return (c.red + c.green + c.blue) / 3.0f;
}

inline RGB32 ColorScalar(const RGB32& c, float scalar)
{
	RGB32 result;
	result.red = c.red * scalar;
	result.green = c.green * scalar;
	result.blue = c.blue * scalar;
	result.alpha = c.alpha * scalar;
	return result;
}

inline RGB32 ColorAdd(const RGB32& c1, const RGB32& c2)
{
	RGB32 result;
	result.red = c1.red + c2.red;
	result.green = c1.green + c2.green;
	result.blue = c1.blue + c2.blue;
	result.alpha = c1.alpha;
	return result;
}

inline RGB32 ColorMultiply(const RGB32& c1, const RGB32& c2)
{
	RGB32 result;
	result.red = c1.red * c2.red;
	result.green = c1.green * c2.green;
	result.blue = c1.blue * c2.blue;
	result.alpha = c1.alpha;
	return result;
}

inline RGB32 ColorAverage(const RGB32& c1, const RGB32& c2)
{
	RGB32 result;
	result.red = (c1.red + c2.red) / 2;
	result.green = (c1.green + c2.green) / 2;
	result.blue = (c1.blue + c2.blue) / 2;
	result.alpha = c1.alpha;
	return result;
}

inline RGB32 AlphaBlend(const RGB32& c1, const RGB32& c2)
{
	RGB32 result;
	unsigned char invAlpha = (unsigned char)255 - c2.alpha;
	result.red = (c2.red * c2.alpha + c1.red * invAlpha) >> 8;
	result.green = (c2.green * c2.alpha + c1.green * invAlpha) >> 8;
	result.blue = (c2.blue * c2.alpha + c1.blue * invAlpha) >> 8;
	return result;
}

inline Vec3 ColorScalarV(const RGB32& c, float scalar)
{
	Vec3 result;
	result.x = float(c.red) * scalar;
	result.y = float(c.green) * scalar;
	result.z = float(c.blue) * scalar;
	return result;
}

inline Vec3 ColorAddV(const RGB32& c, const Vec3& v)
{
	Vec3 result;
	result.x = float(c.red) + v.x;
	result.y = float(c.green) + v.y;
	result.z = float(c.blue) + v.z;
	return result;
}

inline void ColorAddV(RGB32* c, const Vec3& v)
{
	c->red = float(c->red) + v.x;
	c->green = float(c->green) + v.y;
	c->blue = float(c->blue) + v.z;
}

inline Vec3 ColorMultiplyV(const RGB32& c, const Vec3& v)
{
	Vec3 result;
	result.x = float(c.red) * v.x;
	result.y = float(c.green) * v.y;
	result.z = float(c.blue) * v.z;
	return result;
}

const RGB32 BLACK  ( 0,0,0,255 );
const RGB32 WHITE  ( 255,255,255,255 );
const RGB32 GRAY   ( 128,128,128,255 );
const RGB32 RED    ( 255,0,0,255 );
const RGB32 GREEN  ( 0,255,0,255 );
const RGB32 BLUE   ( 0,0,255,255 );
const RGB32 YELLOW ( 255,255,0,255 );
const RGB32 ORANGE ( 255,111,0,255 );
const RGB32 BROWN  ( 139,69,19,255 );
const RGB32 PURPLE ( 127,0,255,255 );
const RGB32 AQUA   ( 0,255,255,255 );
const RGB32 VIOLET ( 204,0,204,255 );

const cl_float4 CL_BLACK  = { 0.0f,0.0f,0.0f,1.0f };
const cl_float4 CL_WHITE    = { 1.0f,1.0f,1.0f,1.0f };
const cl_float4 CL_GRAY     = { 0.5f,0.5f,0.5f,1.0f };
const cl_float4 CL_RED      = { 1.0f,0.0f,0.0f,1.0f };
const cl_float4 CL_GREEN    = { 0.0f,1.0f,0.0f,1.0f };
const cl_float4 CL_BLUE     = { 0.0f,0.0f,1.0f,1.0f };
const cl_float4 CL_YELLOW   = { 1.0f,1.0f,0.0f,1.0f };
const cl_float4 CL_ORANGE   = { 1.0f,0.46f,0.0f,1.0f };
const cl_float4 CL_BROWN    = { 0.55f,0.27f,0.07f,1.0f };
const cl_float4 CL_PURPLE   = { 0.5f,0.0f,1.0f,1.0f };
const cl_float4 CL_AQUA     = { 0.0f,1.0f,1.0f,1.0f };
const cl_float4 CL_VIOLET   = { 0.8f,0.0f,0.8f,1.0f };

/*const RGB32 SUN_RISE_COLOR  (164,126,112,255);
const RGB32 SUN_SET_COLOR   (182,126, 91,255);
const RGB32 SUN_NOON_COLOR  (192,191,173,255);
const RGB32 SUN_BLAZE_COLOR (212,211,193,255);
const RGB32 SUN_HAZEY_COLOR (189,190,192,255);
const RGB32 SUN_CLOUDY_COLOR(174,183,190,255);*/
