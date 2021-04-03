#pragma once
#include <cmath>
#include <cfloat>
#include <algorithm>

#define TWO_PI	6.283185307179586
#define HALF_PI 1.570796326794896
#define PI 3.14159265358979323846
#define DEG2RAD 0.01745329251994329576923 /* pi/180 */

struct float2 {
	float x;
	float y;
};

struct float3 {
	float x;
	float y;
	float z;
};

struct float4 {
	float x;
	float y;
	float z;
	float w;
};

struct int2 {
	int32_t x;
	int32_t y;
};

struct int3 {
	int32_t x;
	int32_t y;
	int32_t z;
};

struct int4 {
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t w;
};

struct uint2 {
	uint32_t x;
	uint32_t y;
};

struct uint3 {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

struct uint4 {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
};

template<typename T>
inline void NormAngle(T& angle)
{
	while (angle < 0.0) { angle += TWO_PI; }
	while (angle > TWO_PI) { angle -= TWO_PI; }
}

template<typename T>
inline T LimitAngle(T angle)
{
	while (angle > TWO_PI) { angle -= TWO_PI; }
	return angle;
}

template<typename T>
inline void SwapVars(T& x1, T& x2)
{
	T temp = x1;
	x1 = x2;
	x2 = temp;
}

template <typename T>
inline T clamp(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

inline double DblFromLong(uint64_t seed) {
	return ((seed%2) ? 1.0 : -1.0) * ((double)seed / std::numeric_limits<uint64_t>::max());
}

inline uint64_t RandomLong(uint64_t seed) {
	seed ^= seed >> 12;
	seed ^= seed << 25;
	seed ^= seed >> 27;
	return seed * 0x2545F4914F6CDD1D;
}

inline float smoothstep(float edge0, float edge1, float x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

inline double smoothstep(double edge0, double edge1, double x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return x * x * (3.0 - 2.0 * x);
}

inline uint32_t Index3Dto1D(const uint3& index, const uint32_t& span)
{
	return index.x + span * (index.y + span * index.z);
}

inline uint64_t Index3Dto1D_64(const uint3& index, const uint32_t& span)
{
	return (uint64_t)index.x + span * (index.y + span * index.z);
}

inline uint3 Index1Dto3D(const uint32_t& index, const uint32_t& span)
{
    uint3 result;
    result.x = index % span;
    result.y = (index / span) % span;
    result.z = index / (span * span);
	return result;
}

inline uint2 StrToUInt2(std::string line)
{
	size_t bpos;
	std::string data;
	uint2 result;
	bpos = line.find(",");
	result.x = stoi(line.substr(0, bpos));
	result.y = stoi(line.substr(bpos+1));
	return result;
}

inline uint3 StrToUInt3(std::string line)
{
	size_t bpos;
	std::string data;
	uint3 result;
	bpos = line.find(",");
	result.x = stoi(line.substr(0, bpos));
	data = line.substr(bpos+1);
	bpos = data.find(",");
	result.y = stoi(data.substr(0, bpos));
	result.z = stoi(data.substr(bpos+1));
	return result;
}

inline float2 StrToFlt2(std::string line)
{
	size_t bpos;
	float2 result;
	bpos = line.find(",");
	result.x = stof(line.substr(0, bpos));
	result.y = stof(line.substr(bpos+1));
	return result;
}

inline float3 StrToFlt3(std::string line)
{
	size_t bpos;
	std::string data;
	float3 result;
	bpos = line.find(",");
	result.x = stof(line.substr(0, bpos));
	data = line.substr(bpos+1);
	bpos = data.find(",");
	result.y = stof(data.substr(0, bpos));
	result.z = stof(data.substr(bpos+1));
	return result;
}

inline uint8_t LifeLvlToEnum(uint8_t level)
{
	if (level < 42) {
		return 0;// no life
	} else if (level < 84) {
		return 1;// single-celled life
	} else if (level < 128) {
		return 2;// simple multi-cell life
	} else if (level < 160) {
		return 3;// complex multi-cell life
	} else if (level < 192) {
		return 4;// primitive civilization
	} else if (level < 224) {
		return 5;// type 0 civilization
	} else if (level < 240) {
		return 6;// type 1 civilization
	} else if (level < 252) {
		return 7;// type 2 civilization
	} else {
		return 8;// type 3 civilization
	}
}
