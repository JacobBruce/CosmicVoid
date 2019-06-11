// ------------------------------ //
// ------- GLOBAL DEFINES ------- //
// ------------------------------ //
//values are in SI units where applicable

#define SECTOR_SPAN (uint)32
#define GALAXY_SPAN (long)2642245
#define LIGHT_YEAR  (double)9.4607304725808e15
#define SECTOR_HALF (double)7.568e17
#define ZONE_WIDTH  (double)4.73e16
#define ZONE_HALF   (double)2.365e16
#define ZONE_SPLIT  (double)2.5e15
#define ALMOST_TWO  (float)1.999999
#define NOISE_MAX   (uchar)6
#define PI_MUL_4o3  4.18879020478639
#define PI_MUL_4    12.5663706143591
#define PI_MUL_2    6.28318530717958
#define ONE_AU      149597870700.0
#define G           6.674e-11

#define MAX_PLANETS 10
#define MAX_MOONS   4

#define EARTH_AGE  4543.0f
#define EARTH_MASS 5.972e24
#define EARTH_DENS 5513.58f
#define EARTH_RADI 6.3781e6
#define EARTH_TEMP 287.75f

#define SOLAR_MASS 2e30
#define SOLAR_RADI 69551e4
#define SOLAR_TEMP 5778.0f
#define SOLAR_LUMI 3.828e26

#define MASS_INDEX 0
#define RADI_INDEX 1
#define TEMP_INDEX 2
#define MVAR_INDEX 3
#define RVAR_INDEX 4
#define TVAR_INDEX 5
#define MAXM_INDEX 6
#define MINR_INDEX 7

#define RED_RGB     245,100,40
#define ORANGE_RGB  245,180,40
#define YELLOW_RGB  245,245,55
#define LYELLOW_RGB 252,252,240
#define BROWN_RGB   120,55,5
#define BLUE_RGB    40,140,245
#define LBLUE_RGB   240,245,252
#define BLACK_RGB   0,0,0
#define VOLC_RGB    216,216,82
#define LAVA_RGB    170,98,10
#define ICE_RGB     176,228,232
#define WATER_RGB   110,180,210
#define EARTH_RGB   202,218,224
#define ROCK_RGB    200,200,200
#define GAS_RGB     202,208,162
#define IRON_RGB    150,124,20
#define CARBON_RGB  60,48,30
#define DESERT_RGB  168,108,48
#define SILIC_RGB   152,158,160

#define STAR_MAG 10.0f
#define PLANET_MAG 1000.0f
#define MOON_TEMP 100.0f
#define LAVA_MIN_TEMP 1000.0f
#define FREEZING_TEMP 273.15f
#define BOILING_TEMP 373.15f
#define FLARE_VIS_DEPTH 0.001f
#define STAR_TEX_RATIO 4.0f
#define STAR_LIGHT_POW 0.001f
#define ISL_DIST_MULT 0.00000005
#define IBUFF_OFFSET 999
#define MAX_MILL_SPECIES 20

// ------------------------------ //
// ------ CONSTANT GLOBALS ------ //
// ------------------------------ //

//diffuse rgb, ambient light
__constant float4 moonDiffData[4] = {
	(float4)(1.0f,1.0f,1.0f, 0.05f),
	(float4)(0.5f,0.5f,0.5f, 0.01f),
	(float4)(0.2f,0.2f,0.2f, 0.01f),
	(float4)(0.9f,0.9f,0.9f, 0.01f)
};

//specular rgb, roughness
__constant float4 moonSpecData[4] = {
	(float4)(0.1f,0.1f,0.1f, 90.0f),
	(float4)(0.9f,0.9f,0.9f, 60.0f),
	(float4)(0.8f,0.8f,0.8f, 70.0f),
	(float4)(0.2f,0.2f,0.2f, 80.0f)
};

//diffuse rgb, ambient light
__constant float4 planetDiffData[12] = {
	(float4)(0.9f,0.9f,0.9f, 0.01f),
	(float4)(1.0f,1.0f,1.0f, 0.01f),
	(float4)(1.0f,1.0f,1.0f, 0.01f),
	(float4)(0.5f,0.5f,0.5f, 0.01f),
	(float4)(0.5f,0.5f,0.5f, 0.01f),
	(float4)(0.2f,0.2f,0.2f, 0.01f),
	(float4)(0.8f,0.8f,0.8f, 0.01f),
	(float4)(0.9f,0.9f,0.9f, 0.01f),
	(float4)(0.8f,0.8f,0.8f, 0.01f),
	(float4)(1.0f,1.0f,1.0f, 0.05f),
	(float4)(1.0f,1.0f,1.0f, 0.01f),
	(float4)(0.6f,0.6f,0.6f, 0.05f)
};

//specular rgb, roughness
__constant float4 planetSpecData[12] = {
	(float4)(0.2f,0.2f,0.2f, 80.0f),
	(float4)(0.1f,0.1f,0.1f, 95.0f),
	(float4)(0.1f,0.1f,0.1f, 95.0f),
	(float4)(0.9f,0.9f,0.9f, 60.0f),
	(float4)(0.9f,0.9f,0.9f, 60.0f),
	(float4)(0.8f,0.8f,0.8f, 70.0f),
	(float4)(0.3f,0.3f,0.3f, 85.0f),
	(float4)(0.1f,0.1f,0.1f, 90.0f),
	(float4)(0.2f,0.2f,0.2f, 80.0f),
	(float4)(0.05f,0.05f,0.05f, 90.0f),
	(float4)(0.01f,0.01f,0.01f, 99.0f),
	(float4)(0.5f,0.5f,0.5f, 75.0f)
};

__constant uchar4 moonAvgColors[4] = {
	(uchar4)(VOLC_RGB,  255),
	(uchar4)(ICE_RGB,   255),
	(uchar4)(WATER_RGB, 255),
	(uchar4)(ROCK_RGB,  255)
};

__constant uchar4 planetAvgColors[12] = {
	(uchar4)(ROCK_RGB,    255),
	(uchar4)(GAS_RGB,     255),
	(uchar4)(GAS_RGB,     255),
	(uchar4)(ICE_RGB,     255),
	(uchar4)(ICE_RGB,     255),
	(uchar4)(WATER_RGB,   255),
	(uchar4)(EARTH_RGB,   255),
	(uchar4)(DESERT_RGB,  255),
	(uchar4)(SILIC_RGB,   255),
	(uchar4)(LAVA_RGB,    255),
	(uchar4)(CARBON_RGB,  255),
	(uchar4)(IRON_RGB,    255)
};

//rgb color, rgb variance, glow intensity, shadow intensity
__constant uchar8 starAvgColors[16] = {
	(uchar8)(RED_RGB,     10,10,10, 60,20),
	(uchar8)(ORANGE_RGB,  10,10,10, 70,20),
	(uchar8)(YELLOW_RGB,  5,5,10,   80,25),
	(uchar8)(LYELLOW_RGB, 3,3,10,   30,10),
	(uchar8)(BROWN_RGB,   15,10,5,  20,10),
	(uchar8)(RED_RGB,     10,10,10, 50,15),
	(uchar8)(YELLOW_RGB,  5,5,10,   90,30),
	(uchar8)(BLUE_RGB,    10,10,10, 60,20),
	(uchar8)(RED_RGB,     10,10,10, 70,25),
	(uchar8)(YELLOW_RGB,  5,5,10,   120,40),
	(uchar8)(BLUE_RGB,    10,10,10, 90,30),
	(uchar8)(RED_RGB,     10,10,10, 100,35),
	(uchar8)(YELLOW_RGB,  5,5,10,   150,50),
	(uchar8)(BLUE_RGB,    10,10,10, 120,40),
	(uchar8)(LBLUE_RGB,   10,5,3,   100,20),
	(uchar8)(BLACK_RGB,   2,2,2,    1,1)
};

//min age, variance (millions of years)
__constant ushort2 starAgeLimits[16] = {
	(ushort2)(100,  8000),
	(ushort2)(100,  6000),
	(ushort2)(100,  4500),
	(ushort2)(6000, 6000),
	(ushort2)(100,  13000),
	(ushort2)(4000, 3000),
	(ushort2)(2500, 2100),
	(ushort2)(500,  1000),
	(ushort2)(4000, 2500),
	(ushort2)(2500, 1500),
	(ushort2)(1,    500),
	(ushort2)(4000, 2000),
	(ushort2)(2500, 1000),
	(ushort2)(1,    200),
	(ushort2)(6000, 2000),
	(ushort2)(1000, 10000)
};

//sol mass, sol radius, sol temp, variances, max mass, min radi
__constant float8 starTypeStats[16] = {
	(float8)(0.6,1.1,0.55,      0.15,0.09,0.15,      0.75,1.01),//red dwarf 0
	(float8)(0.65,0.8,0.8,      0.15,0.15,0.1,       0.8,0.65),//orange dwarf 1
	(float8)(1.0,1.0,1.0,       0.15,0.1,0.05,       1.15,0.9),//yellow dwarf 2
	(float8)(0.7,0.1,0.8,       0.5,0.05,0.3,        1.2,0.005),//white dwarf 3
	(float8)(0.45,0.1,0.3,      0.3,0.05,0.2,        0.75,0.05),//brown dwarf 4
	(float8)(0.4,1.5,0.7,       0.37,0.3,0.15,       0.77,1.2),//red giant 5
	(float8)(5.5,1.8,1.15,      5.0,0.4,0.2,         10.5,1.4),//yellow giant 6
	(float8)(3.0,2.0,1.5,       1.0,0.5,0.25,         4.0,1.5),//blue giant 7
	(float8)(25.0,7.0,0.6,      15.0,0.9,0.15,       40.0,6.1),//red supergiant 8
	(float8)(12.5,5.0,1.1,      12.1,0.7,0.25,       24.6,4.3),//yellow supergiant 9
	(float8)(50.0,3.0,1.8,      40.0,0.5,0.3,        90.0,2.5),//blue supergiant 10
	(float8)(30.0,10.0,0.8,     20.0,1.4,0.2,        50.0,9.6),//red hypergiant 11
	(float8)(25.0,8.0,1.1,      15.0,1.2,0.3,        40.0,6.8),//yellow hypergiant 12
	(float8)(100.0,6.0,2.0,     50.0,1.0,0.4,        150.0,5.0),//blue hypergiant 13
	(float8)(1.7,1.5e-5,100.0,  0.6,5e-6,20.0,       2.3,1e-5),//neutron star 14
	(float8)(350.0,1.42e-3,1e-8,345.0,1.4e-3,-9.9e-9,695.0,2e-5)//black hole 15
};

//density, radius, variances
__constant float4 planetTypeStats[8] = {
	(float4)(0.8,0.4,      0.2,0.1),//Dwarf planet 0
	(float4)(0.3,2.8,      0.15,1.1),//Gas dwarf 1
	(float4)(0.3,8.0,      0.15,4.0),//Gas giant 2
	(float4)(0.8,2.0,      0.3,1.5),//Ice planet 3
	(float4)(0.9,4.0,      0.2,1.0),//Ice giant 4
	(float4)(0.6,1.5,      0.2,1.0),//Water planet 5
	(float4)(1.0,1.0,      0.2,0.5),//Terrestrial planet 6 [Earth-like planet 6 , Desert planet 7, Silicate planet 8]
	(float4)(1.3,0.9,      0.2,0.4)//Theoretical planet 7 [Lava planet 9, Carbon planet 10, Iron planet 11]
};

//density, radius, variances
__constant float4 moonTypeStats[4] = {
	(float4)(0.7,0.3,      0.1,0.1),//Volcanic moon 0
	(float4)(0.6,0.25,     0.1,0.1),//Ice moon 1
	(float4)(0.5,0.3,      0.1,0.1),//Water moon 2
	(float4)(0.7,0.25,     0.1,0.1)//Rock moon 3
};

__constant uchar starTypeProbs[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,
	7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,
	9,9,
	10,10,10,10,10,10,
	11,11,
	12,
	13,13,13,13,
	14,14,14,14,14,14,14,14,14,14,
	15,15,15,15,15,15,15,15,15
};

__constant uchar planetTypeProbs[64] = {
	0,0,0,0,
	1,1,1,1,1,1,
	2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7
};
 
__constant uchar moonTypeProbs[32] = {
	0,
	1,1,1,1,1,1,1,1,1,1,
	2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
};

// ------------------------------ //
// ------ TYPE DEFINITIONS ------ //
// ------------------------------ //

#pragma pack(push,1)

enum LifeTypes {
	NO_LIFE=0,
	SINGLE_CELLED=1,
	SIMPLE_LIFE=2,
	COMPLEX_LIFE=3,
	PRIMITIVE_CIV=4,
	TYPE_0_CIV=5,
	TYPE_I_CIV=6,
	TYPE_II_CIV=7,
	TYPE_III_CIV=8
};

enum MoonFuncs {
	NOT_USED=0,
	PIRATE_BASE=1,
	WASTE_DISPOSAL=2,
	COLONIZATION=3,
	INDUSTRIALIZATION=4,
	COMMERCIALIZATION=5,
	PLANET_DEFENSE=6,
	SCIENTIFIC_RESEARCH=7,
	TRADING_HUB=8,
	PLANET_ENTRY_PORT=9
};

typedef struct {
	union {
		struct {
			uchar red;
			uchar green;
			uchar blue;
			uchar alpha;
		};
		uchar4 rgba;
		uint data;
	};
} RGB32;

typedef struct {
	union {
		RGB32 colors[4];
		uint4 data;
	};
} PFrags;

typedef struct {
	union {
		uchar bytes[4];
		ulong data;
	};
} LongBytes;

typedef struct {
	union {
		uchar bytes[2];
		ushort data;
	};
} ShortBytes;

typedef struct {
	union {
		float s[8];
		float8 data;
	};
} FloatVec8;

typedef struct {
	union {
		uchar s[8];
		uchar8 data;
	};
} UCharVec8;

typedef struct {
	union {
		float s[4];
		float4 data;
	};
} FloatVec4;

typedef struct {
	union {
		uchar s[4];
		uchar4 data;
	};
} UCharVec4;

typedef struct {
	double3 bl_ray;
	double3 cam_pos;
	double3 cam_ori;
	double3 cam_fwd;
	double3 cam_rgt;
	double3 cam_up;
	double cam_foc;
	double d_time;
	uint3 zone_offset;
	uint pixels_X;
	uint pixels_Y;
	uint half_X;
	uint half_Y;
	uint rand_int;
	float aa_inc;
	float aa_div;
	short aa_lvl;
	short aa_dim;
} RenderInfo;

typedef struct {
	union {
		struct {
			float3 point;
			float3 normal;
		};
		struct {
			float pnt[3];
			float depth;
			float nrm[3];
			uint index;
		};
	};
} RayIntersect;

typedef struct {
	float4 color;
	float2 offset;
	float scale;
	float depth;
} InstanceData;

typedef struct {
    union {
        double3 position;
        struct {
            double x_pos;
            double y_pos;
            double z_pos;
            ushort age;
			uchar oxygen;
			uchar carbon;
			uchar hydrog;
			uchar helium;
			uchar metal;
            uchar type;
        };
    };
	float mass;
	float luminosity;
	float radius;
	float density;
	float volume;
	float area;
	float temp;
	RGB32 color;
} Star;

typedef struct {
    union {
        double3 position;
        struct {
            double x_pos;
            double y_pos;
            double z_pos;
			union {
				uchar8 info;
				struct {
					ushort age;
					uchar moons;
					uchar species;
					uchar hostility;
					uchar habitability;
					uchar life_level;
					uchar type;
				};
			};
        };
    };
	float3 orbit;
	float mass;
	float radius;
	float density;
	float volume;
	float area;
	float temp;
	float water_frac;
	float rot_time;
	float orbit_time;
	float orbit_rad;
	float eccentricity;
	float velocity;
} Planet;

typedef struct {
    union {
        double3 position;
        struct {
            double x_pos;
            double y_pos;
            double z_pos;
			union {
				uchar8 info;
				struct {
					ushort age;
					uchar function;
					uchar species;
					uchar hostility;
					uchar habitability;
					uchar life_level;
					uchar type;
				};
			};
        };
    };
	float3 orbit;
	float mass;
	float radius;
	float density;
	float volume;
	float area;
	float temp;
	float water_frac;
	float rot_time;
	float orbit_time;
	float orbit_rad;
	float eccentricity;
	float velocity;
} Moon;

typedef struct {
	double3 plane;
	ulong index;
	uint conflict;
	uint economy;
	uint security;
	uint civ_count;
	uint moons;
	uint planets;
} SolarSystem;

#pragma pack(pop)

// ------------------------------ //
// ------ VECTOR FUNCTIONS ------ //
// ------------------------------ //

float Sqrd2D(const float2 v1, const float2 v2)
{
	return pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2);
}
float Dist2D(const float2 v1, const float2 v2)
{
	return sqrt(Sqrd2D(v1, v2));
}

double VectMag(const double3 v)
{
	return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
double VectSqrd(const double3 v1, const double3 v2)
{
	return pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2) + pow(v1.z-v2.z, 2);
}
double VectDist(const double3 v1, const double3 v2)
{
	return sqrt(VectSqrd(v1, v2));
}
double3 VectRotX(const double3 v, const double angle)
{
	return (double3)(v.x, (v.z*sin(angle)) + (v.y*cos(angle)), (v.z*cos(angle)) - (v.y*sin(angle)));
}
double3 VectRotY(const double3 v, const double angle)
{
	return (double3)((v.z*sin(angle)) + (v.x*cos(angle)), v.y, (v.z*cos(angle)) - (v.x*sin(angle)));
}
double3 VectRotZ(const double3 v, const double angle)
{
	return (double3)((v.x*cos(angle)) - (v.y*sin(angle)), (v.x*sin(angle)) + (v.y*cos(angle)), v.z);
}
double3 VectRot(const double3 v, const double3 rot)
{
	double3 result = VectRotY(v, rot.y);
	result = VectRotZ(result, rot.z);
	return VectRotX(result, rot.x);
}
double3 VectFloor(const double3 v)
{
	return (double3)(floor(v.x), floor(v.y), floor(v.z));
}

// ----------------------------- //
// ------ COLOR FUNCTIONS ------ //
// ----------------------------- //

char IsBlack(const RGB32 color) {
	if (color.red == 0 && color.green == 0 && color.blue == 0) {
		return 1;
	} else {
		return 0;
	}
}

RGB32 AlphaBlend(const RGB32 c1, const RGB32 c2)
{
	RGB32 result;
	unsigned char invAlpha = (unsigned char)255 - c2.alpha;
	result.red = (c2.red * c2.alpha + c1.red * invAlpha) >> 8;
	result.green = (c2.green * c2.alpha + c1.green * invAlpha) >> 8;
	result.blue = (c2.blue * c2.alpha + c1.blue * invAlpha) >> 8;
	return result;
}

float4 NormalizeColor(const float3 v, const float a)
{
	return (float4)(v / 255.0f, a);
}

float3 ColorToVec3(const RGB32 c)
{
	return (float3)(c.red/255.0f, c.green/255.0f, c.blue/255.0f);
}

float4 ColorToVec4(const RGB32 c)
{
	return (float4)(c.red/255.0f, c.green/255.0f, c.blue/255.0f, c.alpha/255.0f);
}

RGB32 ScaleColor(RGB32 color, float g_frac)
{
	color.red = round(fmin(color.red * g_frac, 255.0f));
	color.green = round(fmin(color.green * g_frac, 255.0f));
	color.blue = round(fmin(color.blue * g_frac, 255.0f));
	return color;
}

RGB32 ColorMultiplyV(uchar4 c, float3 v) 
{
	RGB32 result = {(uchar)min(c.x*v.x, 255.0f), 
					(uchar)min(c.y*v.y, 255.0f),
					(uchar)min(c.z*v.z, 255.0f), 255};
	return result;
}

// ------------------------------ //
// ------- MISC FUNCTIONS ------- //
// ------------------------------ //

double RaySphereIntersect(const double3 orig, const double3 dir, const double3 pos, const double rad2)
{
	double t0, t1;
	double3 L = pos - orig;
	double tca = dot(L, dir);
	
	if (tca < 0.0) { return 0.0; }
	double d2 = dot(L, L) - tca * tca;
	if (d2 > rad2) { return 0.0; }
	double thc = sqrt(rad2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) {
		double temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if (t0 < 0.0 && t1 < 0.0) return 0.0;

	return t0;
}

float3 CalcSurfColor(double3 surf_pnt, double3 norm_vec, double3 cam_pos, double3 light_pos, float3 light_color, float4 diffuse, float4 specular, float light_power)
{
	// get distance between light and planet surface
	double sDist = distance(surf_pnt, light_pos);

	// get light direction ray relative to surface
	double3 losVect = normalize(light_pos - surf_pnt);

	// get camera direction ray relative to surface
	double3 cosVect = normalize(cam_pos - surf_pnt);

	// apply Lambertian diffuse reflection to light intensity
	float dotProd = max(dot(losVect, norm_vec), 0.0);
	float3 ltiVect = (light_color * (diffuse.xyz * dotProd)) + diffuse.w;

	// apply Blinn-Phong specular reflection to light intensity
	if (dotProd > 0.0f) {
		double3 hlfVect = normalize((losVect + cosVect) * 0.5);
		dotProd = pow((float)dot(hlfVect, norm_vec), specular.w);
		ltiVect += specular.xyz * dotProd;
	}

	// apply inverse square law to light intensity
	return ltiVect * (float)min(1.0, light_power/pow(sDist, 2.0));
}


double RandDblFromLng(ulong seed) {
	return ((seed%2) ? 1.0 : -1.0) * ((seed % UINT_MAX) / (UINT_MAX-1.0));
}

ulong RandomLong(ulong seed) {
	seed ^= seed >> 12;
	seed ^= seed << 25;
	seed ^= seed >> 27;
	return seed * 0x2545F4914F6CDD1D;
}

double AbsFract(double num) {
	double an = fabs(num);
	return an - floor(an);
}

float noise(double4 seed) {
    return AbsFract(sin(dot(seed,(double4)(13.095,78.233,64.123, 5.739))) * 48791.46359);
}

uint increment(volatile __global uint* counter)
{
	return atomic_inc(counter);
}

uint decrement(volatile __global uint* counter)
{
	return atomic_dec(counter);
}

float StarGlowSize(double luminosity, double distance)
{
	return log(1.0 + (luminosity / (ISL_DIST_MULT * pow(distance, 2.0))));
}

uchar BaseMoonHabScore(uchar moon_type, ulong rnd)
{
	switch (moon_type) {
		case 0: return 5  + (rnd%3);
		case 1: return 30 + (rnd%3);
		case 2: return 64 + (rnd%3);
		case 3: return 20 + (rnd%3);
		default: return 0;
	}
}

float GetMoonWaterFrac(uchar moon_type, float planet_frac, ulong rnd)
{
	switch (moon_type) {
		case 0: return (0.0001f + (0.00001*(rnd%9))) * planet_frac;
		case 1: return (0.4 + (0.05*(rnd%9))) * planet_frac;
		case 2: return (0.8 + (0.1*(rnd%9))) * planet_frac;
		case 3: return (0.1 + (0.01*(rnd%9))) * planet_frac;
		default: return 0.0f;
	}
}

uchar LifeLvlToEnum(uchar level)
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

// ------------------------------ //
// ------ KERNEL FUNCTIONS ------ //
// ------------------------------ //

__kernel void GenStars(__global Star* star_buffer, const RenderInfo render_info)
{
    uint zone_X = get_global_id(0);
	uint zone_Y = get_global_id(1);
	uint zone_Z = get_global_id(2);
	uint zone_index = zone_X + SECTOR_SPAN * (zone_Y + SECTOR_SPAN * zone_Z);
	
    long glob_X = (render_info.zone_offset.x + zone_X) % GALAXY_SPAN;
	long glob_Y = (render_info.zone_offset.y + zone_Y) % GALAXY_SPAN;
	long glob_Z = (render_info.zone_offset.z + zone_Z) % GALAXY_SPAN;
	ulong glob_index = glob_X + GALAXY_SPAN * (glob_Y + GALAXY_SPAN * glob_Z);
	
	double offset_X = (15.0 - (double)zone_X) * ZONE_WIDTH;
	double offset_Y = (15.0 - (double)zone_Y) * ZONE_WIDTH;
	double offset_Z = (15.0 - (double)zone_Z) * ZONE_WIDTH;

	double variFrac, sM, sT, sR;
	float denseFrac, maxDF;
	FloatVec8 starStats;
	UCharVec8 starColDat;
	LongBytes seed;
	ulong4 seeds;
	Star star;

	seeds.x = RandomLong(glob_index);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	seed.data = seeds.x;
	star.type = starTypeProbs[seed.bytes[3]];
	starStats.data = starTypeStats[star.type];
	starColDat.data = starAvgColors[star.type];

	variFrac = RandDblFromLng(seeds.y);
	sM = starStats.s[MASS_INDEX] + (starStats.s[MVAR_INDEX] * variFrac);
	variFrac = RandDblFromLng(seeds.z);
	sR = starStats.s[RADI_INDEX] + (starStats.s[RVAR_INDEX] * variFrac);
	
	star.age = starAgeLimits[star.type][0] + (seeds.w % starAgeLimits[star.type][1]);
	star.mass = SOLAR_MASS * sM;
	star.radius = SOLAR_RADI * sR;
	star.area = PI_MUL_4 * pow(star.radius, 2.0f);
	star.volume = PI_MUL_4o3 * pow(star.radius, 3.0f);
	star.density = star.mass / star.volume;

	denseFrac = sM / sR;
	maxDF = starStats.s[MAXM_INDEX] / starStats.s[MINR_INDEX];
	variFrac = denseFrac / maxDF;
	sT = starStats.s[TEMP_INDEX] + (starStats.s[TVAR_INDEX] * variFrac);
	star.temp = SOLAR_TEMP * sT;
	star.luminosity = SOLAR_LUMI * pow(sR,2.0) * pow(sT,4.0);
	
	seed.data = seeds.w;
	star.color.red = starColDat.s[0] + (seed.bytes[0] % starColDat.s[3]);
	star.color.green = starColDat.s[1] + (seed.bytes[1] % starColDat.s[4]);
	star.color.blue = starColDat.s[2] + (seed.bytes[2] % starColDat.s[5]);
	star.color.alpha = 255;

	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	
	star.position.x = offset_X + ((double)(seeds.x % 16UL) * ZONE_SPLIT);
	star.position.y = offset_Y + ((double)(seeds.y % 16UL) * ZONE_SPLIT);
	star.position.z = offset_Z + ((double)(seeds.z % 16UL) * ZONE_SPLIT);
	
	star_buffer[zone_index] = star;
}

__kernel void GenPlanets(__global SolarSystem* solar_system,
__global Planet* planet_buffer, __global Moon* moon_buffer,
__global Star* star_buffer, __global uint* loaded_star)
{
    uint index = get_global_id(0);
	ulong seed = solar_system->index * (1+index);

	ShortBytes age;
	uchar moons, function, species, hostility;
	uchar habitability, life_level, type;
	
	double pD, pR;
	float variFrac, lumiFrac;
	float frac1, frac2, randFlt;
	uint mindex, mstart;
	uint sumHostile;
	FloatVec4 bodyStats;
	uchar lifeType;
	ulong4 seeds;
	Planet planet;
	Moon moon;
	
	Star star = star_buffer[*loaded_star];

	seeds.x = RandomLong(seed);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	type = planetTypeProbs[seeds.x%64];
	bodyStats.data = planetTypeStats[type];

	variFrac = RandDblFromLng(seeds.y);
	pD = bodyStats.s[0] + (bodyStats.s[2] * variFrac);
	variFrac = RandDblFromLng(seeds.z);
	pR = bodyStats.s[1] + (bodyStats.s[3] * variFrac);
	
	planet.density = EARTH_DENS * pD;
	planet.radius = EARTH_RADI * pR;
	planet.area = PI_MUL_4 * pow(planet.radius, 2.0f);
	planet.volume = PI_MUL_4o3 * pow(planet.radius, 3.0f);
	planet.mass = planet.density * planet.volume;
	
	lumiFrac = star.luminosity / SOLAR_LUMI;
	frac1 = max(planet.mass / EARTH_MASS, 0.3);
	frac2 = (star.mass / SOLAR_MASS) * 1000.0f;
	planet.eccentricity = 0.00001 + (fabs(RandDblFromLng(seeds.x)) * ((seeds.x%4) ? 0.01 : 0.49));
	planet.velocity = 1000.0 + frac2 + (fabs(RandDblFromLng(seeds.w) * 60000.0) / frac1);
	planet.orbit_rad = G * star.mass / pow(planet.velocity, 2.0f);
	planet.temp = fmax(5.0, lumiFrac * (EARTH_TEMP / pow(planet.orbit_rad / ONE_AU, 2.0)));
    planet.orbit_time = (PI_MUL_2 * planet.orbit_rad) / planet.velocity;
	planet.rot_time = 21600.0 + ((seeds.w % 3600000) / frac1);
	planet.position.xyz = star.position.xyz;
	planet.position.x += planet.orbit_rad;

	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	if (type == 3 && type == 4) {
		if (planet.temp > FREEZING_TEMP) {
			if (planet.temp < BOILING_TEMP) {
				type = 5;
			} else {
				type = (seeds.x%2) ? 6 : 7;
			}
		}
	}
	
	if (type == 5) {
		if (planet.temp < FREEZING_TEMP) {
			type = 3;
		} else if (planet.temp > BOILING_TEMP) {
			type = (seeds.x%2) ? 6 : 7;
		}
	}

	if (type == 7) {
		if (planet.temp > LAVA_MIN_TEMP) {
			type = 9;
		} else {
			type = 10 + (seeds.w%2);
		}
	}
	
	if (type == 6) {
		if (planet.temp > FREEZING_TEMP && planet.temp < BOILING_TEMP) {
			type = 6;
		} else {
			type = 7 + (seeds.w%2);
		}
	}
	
	switch (type) {
	case 0: //Dwarf planet
		variFrac = 0.1f;
		planet.water_frac = 0.0001f;
		planet.temp *= (90 + (seeds.y%20)) / 100.0f;
		habitability = 40 + (seeds.z%3);
		break;
	case 1: //Gas dwarf
		variFrac = 0.3f;
		planet.water_frac = 0.001f;
		planet.temp *= (80 + (seeds.y%20)) / 100.0f;
		habitability = 1 + (seeds.z%3);
		break;
	case 2: //Gas giant
		variFrac = 0.3f;
		planet.water_frac = 0.001f;
		planet.temp *= (80 + (seeds.y%20)) / 100.0f;
		habitability = 1 + (seeds.z%3);
		break;
	case 3: //Ice planet
		variFrac = 0.5f;
		planet.water_frac = 0.4f;
		planet.temp *= (80 + (seeds.y%10)) / 100.0f;
		habitability = 30 + (seeds.z%3);
		break;
	case 4: //Ice giant
		variFrac = 0.5f;
		planet.water_frac = 0.4f;
		planet.temp *= (80 + (seeds.y%10)) / 100.0f;
		habitability = 30 + (seeds.z%3);
		break;
	case 5: //Water planet
		variFrac = 0.15f;
		planet.water_frac = 0.8f;
		planet.temp *= (90 + (seeds.y%10)) / 100.0f;
		habitability = 64 + (seeds.z%3);
		break;
	case 6: //Earth-like planet
		variFrac = 0.4f;
		planet.water_frac = 0.4f;
		planet.temp *= (98 + (seeds.y%5)) / 100.0f;
		habitability = 125 + (seeds.z%3);
		break;
	case 7: //Desert planet
		variFrac = 0.01f;
		planet.water_frac = 0.00001f;
		planet.temp *= (100 + (seeds.y%10)) / 100.0f;
		habitability = 20 + (seeds.z%3);
		break;
	case 8: //Silicate planet
		variFrac = 0.2f;
		planet.water_frac = 0.0001f;
		planet.temp *= (96 + (seeds.y%9)) / 100.0f;
		habitability = 50 + (seeds.z%3);
		break;
	case 9: //Lava planet
		variFrac = 0.0001f;
		planet.water_frac = 0.000001f;
		planet.temp *= (110 + (seeds.y%10)) / 100.0f;
		habitability = 5 + (seeds.z%3);
		break;
	case 10: //Carbon planet
		variFrac = 0.001f;
		planet.water_frac = 0.00001f;
		planet.temp *= (100 + (seeds.y%10)) / 100.0f;
		habitability = 15 + (seeds.z%3);
		break;
	case 11: //Iron planet
		variFrac = 0.001f;
		planet.water_frac = 0.00001f;
		planet.temp *= (90 + (seeds.y%20)) / 100.0f;
		habitability = 10 + (seeds.z%3);
		break;
	}
	
	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	frac1 = fabs(RandDblFromLng(seeds.x));
	frac2 = 1.0f - fmin(1.0f, planet.temp/LAVA_MIN_TEMP);
	planet.water_frac = (frac2 > 0.0) ? planet.water_frac + (frac2 * frac1 * variFrac) : frac1 * planet.water_frac;
	habitability += (64.0f-(fabs(fmin(2.0f,(planet.temp/EARTH_TEMP))-1.0f)*64.0f)) + fmin(64.0f,planet.water_frac*128.0f);
	age.data = star.age * (0.8 + fabs(RandDblFromLng(seeds.y) * 0.19));
	//TODO:calc terraformed and colonized planets/moons on CPU
	life_level = (seeds.y % habitability) * fmin(1.0f, age.data / EARTH_AGE);
	lifeType = LifeLvlToEnum(life_level);
	species = (lifeType>0) ? seeds.z % life_level : 0;
	hostility = (lifeType > COMPLEX_LIFE) ? (fabs(RandDblFromLng(seeds.z)) * 160.0) + (255-life_level) : 0;
	sumHostile = hostility;
	moons = seeds.w % MAX_MOONS;
	
	planet.info = (uchar8)(age.bytes[0], age.bytes[1], moons, species, hostility, habitability, life_level, type);
	
	frac2 = (planet.mass / EARTH_MASS) * 100.0;
	mstart = index * MAX_MOONS;
	seed = ~seed;
	for (uint m=0; m<moons; ++m) {

		seeds.x = RandomLong(seed * (1+m));
		seeds.y = RandomLong(seeds.x);
		seeds.z = RandomLong(seeds.y);
		seeds.w = RandomLong(seeds.z);
		mindex = mstart + m;

		type = (planet.type==9) ? 0 : moonTypeProbs[seeds.x%32];
		type = (type==1 && planet.type==5) ? 2 : type;
		type = (type==1 && planet.temp > FREEZING_TEMP) ? 3 : type;
		bodyStats.data = moonTypeStats[type];

		variFrac = RandDblFromLng(seeds.y);
		pD = bodyStats.s[0] + (bodyStats.s[2] * variFrac);
		variFrac = RandDblFromLng(seeds.z);
		pR = bodyStats.s[1] + (bodyStats.s[3] * variFrac);

		moon.density = EARTH_DENS * pD;
		moon.radius = EARTH_RADI * pR;
		moon.area = PI_MUL_4 * pow(moon.radius, 2.0f);
		moon.volume = PI_MUL_4o3 * pow(moon.radius, 3.0f);
		moon.mass = moon.density * moon.volume;

		frac1 = min(10.0, (moon.mass / EARTH_MASS) * 100.0);
		moon.eccentricity = 0.000001 + (fabs(RandDblFromLng(seeds.x)) * 0.1);
		moon.velocity = 100.0f + frac2 + (fabs(RandDblFromLng(seeds.w) * 2000.0) / frac1);
		moon.orbit_rad = G * planet.mass / pow(moon.velocity, 2.0f);
		moon.temp = fmax(2.0, lumiFrac * (MOON_TEMP / pow(planet.orbit_rad / ONE_AU, 2.0)));
		moon.orbit_time = (PI_MUL_2 * moon.orbit_rad) / moon.velocity;
		moon.rot_time = 21600.0 + ((seeds.w % 3600000) / frac1);
		moon.position.xyz = planet.position.xyz;
		moon.position.z += (planet.radius * PLANET_MAG) + (moon.orbit_rad * (PLANET_MAG/10.0));

		seeds.x = RandomLong(seeds.w);
		seeds.y = RandomLong(seeds.x);
		seeds.z = RandomLong(seeds.y);
		seeds.w = RandomLong(seeds.z);

		moon.orbit.x = RandDblFromLng(seeds.x);
		moon.orbit.y = RandDblFromLng(seeds.y);
		variFrac = fabs(moon.orbit.x) + fabs(moon.orbit.y);
		moon.orbit.y = (variFrac>1.0f) ? 1.0f - moon.orbit.x : moon.orbit.y;
		moon.orbit.z = (variFrac>1.0f) ? 0.0f : 1.0f - variFrac;

		age.data = planet.age + (planet.age * (RandDblFromLng(seeds.z) * 0.05));
		moon.water_frac = GetMoonWaterFrac(type, planet.water_frac, seeds.z);
		habitability = BaseMoonHabScore(type, seeds.w);
		habitability += (64.0f-(fabs(fmin(2.0f,(moon.temp/EARTH_TEMP))-1.0f)*64.0f)) + fmin(64.0f,moon.water_frac*128.0f);
		hostility = fmin(255.0, (1.0 + (0.1 * RandDblFromLng(seeds.w))) * planet.hostility);
		
		seeds.x = RandomLong(seeds.w);
		seeds.y = RandomLong(seeds.x);
		seeds.z = RandomLong(seeds.y);
		seeds.w = RandomLong(seeds.z);

		if (lifeType < TYPE_0_CIV) {
			function = (fabs(RandDblFromLng(seeds.x)) < 0.05) ? 1 : 0;
			life_level = (seeds.y % habitability) * fmin(1.0f, age.data / EARTH_AGE);
			species = LifeLvlToEnum(life_level) ? seeds.z % 1+MAX_MILL_SPECIES : 0;
		} else if (lifeType < TYPE_I_CIV) {
			function = seeds.x % 4;
			life_level = (function>0) ? planet.life_level : 0;
			species = 0;
		} else {
			function = 4 + (seeds.x % 6);
			life_level = planet.life_level;
			species = 0;
		}

		switch (function) {
		case PIRATE_BASE:
			hostility = min(255, (int)hostility+50);
			break;
		case INDUSTRIALIZATION:
			atomic_add(&solar_system->economy, (lifeType*4) + (seeds.w%lifeType));
			break;
		case COMMERCIALIZATION:
			atomic_add(&solar_system->economy, (lifeType*5) + (seeds.w%lifeType));
			break;
		case TRADING_HUB:
			atomic_add(&solar_system->economy, (lifeType*8) + (seeds.w%lifeType));
			break;
		case PLANET_DEFENSE:
			atomic_add(&solar_system->security, (lifeType*8) + (seeds.w%lifeType));
			break;
		case SCIENTIFIC_RESEARCH:
			atomic_add(&solar_system->security, (lifeType*3) + (seeds.w%lifeType));
			break;
		case PLANET_ENTRY_PORT:
			atomic_add(&solar_system->security, (lifeType*4) + (seeds.w%lifeType));
			break;
		}

		sumHostile += hostility;
		moon.info = (uchar8)(age.bytes[0], age.bytes[1], function, species, hostility, habitability, life_level, type);
		moon_buffer[mindex] = moon;
	}
	
	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);

	if (lifeType >= PRIMITIVE_CIV) {

		atomic_add(&solar_system->civ_count, 1);

		switch (lifeType) {
		case PRIMITIVE_CIV:
			atomic_add(&solar_system->economy, lifeType + (seeds.x%lifeType));
			atomic_add(&solar_system->security, lifeType + (seeds.y%lifeType));
			break;
		case TYPE_0_CIV:
			atomic_add(&solar_system->economy, lifeType + (seeds.x%lifeType));
			atomic_add(&solar_system->security, lifeType + (seeds.y%lifeType));
			break;
		case TYPE_I_CIV:
			atomic_add(&solar_system->economy, lifeType + (seeds.x%lifeType));
			atomic_add(&solar_system->security, lifeType + (seeds.y%lifeType));
			break;
		case TYPE_II_CIV:
			atomic_add(&solar_system->economy, lifeType + (seeds.x%lifeType));
			atomic_add(&solar_system->security, lifeType + (seeds.y%lifeType));
			break;
		case TYPE_III_CIV:
			atomic_add(&solar_system->economy, lifeType + (seeds.x%lifeType));
			atomic_add(&solar_system->security, lifeType + (seeds.y%lifeType));
			break;
		}
	}

	atomic_add(&solar_system->moons, planet.moons);
	atomic_add(&solar_system->conflict, (uint)(fabs(RandDblFromLng(seeds.x))*sumHostile));
	
	planet_buffer[index] = planet;
}

__kernel void DrawMoon(__global Moon* moon_buffer, 
__global Star* star_buffer, __global PFrags* frag_buffer,
const uint moon_index, const uint star_index, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	Star star = star_buffer[star_index];
	Moon moon = moon_buffer[moon_index];
	uchar4 color = moonAvgColors[moon.type];
	
	float3 starClr = ColorToVec3(star.color);
	float4 diffuse = moonDiffData[moon.type];
	float4 specular = moonSpecData[moon.type];

	starClr = fmax(starClr * 2.0f, (float3)(1.0f,1.0f,1.0f));

	double sDist;
	float fakeRad;
	float3 ltiVect;
	double3 wldPnt;
	//double3 camPnt;
	double3 normRay;
	double3 primRay;
	float lightPow = STAR_LIGHT_POW * star.luminosity;
	//RayIntersect intersection;
	
	// loop through pixel fragments
	for (uint rx=0; rx < render_info.aa_dim; ++rx)
	{	
		for (uint ry=0; ry < render_info.aa_dim; ++ry)
		{
			// calculate primary ray for pixel fragment
			primRay = render_info.bl_ray + (render_info.cam_rgt * (pix_X+(render_info.aa_inc*rx)));
			primRay = normalize(primRay + (render_info.cam_up * (pix_Y+(render_info.aa_inc*ry))));
		
			// check if ray intersects with sphere
			fakeRad = moon.radius * PLANET_MAG;
			sDist = RaySphereIntersect(render_info.cam_pos, primRay, moon.position, fakeRad*fakeRad);

			if (sDist > 0.0) {
				// get hit point in camera space
				//camPnt = primRay * sDist;
				// get hit point in world space
				wldPnt = render_info.cam_pos + (primRay * sDist);

				// is hit point in front of camera?
				//if (camPnt.z > 0.0) {	
					// get normal and color for intersection
					normRay = normalize(wldPnt - moon.position);
					ltiVect = CalcSurfColor(wldPnt, normRay, render_info.cam_pos, star.position, starClr, diffuse, specular, lightPow);
					
					frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx] = ColorMultiplyV(color, ltiVect);

					//intersection.point = wldPnt;
					//intersection.normal = normRay;
					//intersection.depth = sDist;
					//intersection.index = moon_index;
				//}
			}
		}
	}
}

__kernel void DrawPlanet(__global Planet* planet_buffer, 
__global Star* star_buffer, __global PFrags* frag_buffer,
const uint planet_index, const uint star_index, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	Star star = star_buffer[star_index];
	Planet planet = planet_buffer[planet_index];
	uchar4 color = planetAvgColors[planet.type];
	
	float3 starClr = ColorToVec3(star.color);
	float4 diffuse = planetDiffData[planet.type];
	float4 specular = planetSpecData[planet.type];
	
	starClr = fmax(starClr * 2.0f, (float3)(1.0f,1.0f,1.0f));

	double sDist;
	float fakeRad;
	float3 ltiVect;
	double3 wldPnt;
	//double3 camPnt;
	double3 normRay;
	double3 primRay;
	float lightPow = STAR_LIGHT_POW * star.luminosity;
	//RayIntersect intersection;
	
	// loop through pixel fragments
	for (uint rx=0; rx < render_info.aa_dim; ++rx)
	{	
		for (uint ry=0; ry < render_info.aa_dim; ++ry)
		{
			// calculate primary ray for pixel fragment
			primRay = render_info.bl_ray + (render_info.cam_rgt * (pix_X+(render_info.aa_inc*rx)));
			primRay = normalize(primRay + (render_info.cam_up * (pix_Y+(render_info.aa_inc*ry))));
		
			// check if ray intersects with sphere
			fakeRad = planet.radius * PLANET_MAG;
			sDist = RaySphereIntersect(render_info.cam_pos, primRay, planet.position, fakeRad*fakeRad);

			if (sDist > 0.0) {
				// get hit point in camera space
				//camPnt = primRay * sDist;
				// get hit point in world space
				wldPnt = render_info.cam_pos + (primRay * sDist);

				// is hit point in front of camera?
				//if (camPnt.z > 0.0) {
					// get normal and color for intersection
					normRay = normalize(wldPnt - planet.position);
					ltiVect = CalcSurfColor(wldPnt, normRay, render_info.cam_pos, star.position, starClr, diffuse, specular, lightPow);

					frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx] = ColorMultiplyV(color, ltiVect);

					//intersection.point = wldPnt;
					//intersection.normal = normRay;
					//intersection.depth = sDist;
					//intersection.index = planet_index;
				//}
			}
		}
	}
}

__kernel void DrawStar(__global Star* star_buffer, __global PFrags* frag_buffer,
const uint star_index, const double game_time, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	Star star = star_buffer[star_index];
	uchar starGlow = starAvgColors[star.type][6];
	uchar starShadow = starAvgColors[star.type][7];

	double sDist;
	float fakeRad;
	double3 wldPnt;
	//double3 camPnt;
	double3 normRay;
	double3 primRay;
	int cSum;
	//RayIntersect intersection;
	
	//TODO: dark sunspots
	
	// loop through pixel fragments
	for (uint rx=0; rx < render_info.aa_dim; ++rx)
	{	
		for (uint ry=0; ry < render_info.aa_dim; ++ry)
		{
			// calculate primary ray for pixel fragment
			primRay = render_info.bl_ray + (render_info.cam_rgt * (pix_X+(render_info.aa_inc*rx)));
			primRay = normalize(primRay + (render_info.cam_up * (pix_Y+(render_info.aa_inc*ry))));
		
			// check if ray intersects with sphere
			fakeRad = star.radius * STAR_MAG;
			sDist = RaySphereIntersect(render_info.cam_pos, primRay, star.position, fakeRad*fakeRad);

			if (sDist > 0.0) {
				// get hit point in camera space
				//camPnt = primRay * sDist;
				// get hit point in world space
				wldPnt = render_info.cam_pos + (primRay * sDist);

				// is hit point in front of camera?
				//if (camPnt.z > 0.0) {	
					// get normal and color for intersection
					normRay = normalize(wldPnt - star.position);
					
					cSum = ((noise((double4)(VectFloor(normRay*500.0),game_time)) + 
							noise((double4)(VectFloor(normRay*250.0),game_time))) * starGlow);

					frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx].rgba = (uchar4)
						(min(max((int)star.color.red+cSum-starShadow, 0), 255),
						 min(max((int)star.color.green+cSum-starShadow, 0), 255),
						 min(max((int)star.color.blue+cSum-starShadow, 0), 255), 255);

					//intersection.point = wldPnt;
					//intersection.normal = normRay;
					//intersection.depth = sDist;
					//intersection.index = star_index;
				//}
			}
		}
	}
}

__kernel void DrawStars(__global Star* star_buffer, __global PFrags* frag_buffer, 
__global InstanceData* idata_buffer, __global uint* adb_sizes, 
__global uint* loaded_star, const RenderInfo render_info)
{
    uint zone_X = get_global_id(0);
	uint zone_Y = get_global_id(1);
	uint zone_Z = get_global_id(2);
	uint zone_index = zone_X + SECTOR_SPAN * (zone_Y + SECTOR_SPAN * zone_Z);
	
	Star star = star_buffer[zone_index];
	RGB32 color, bcol;
	double3 sprc;
	//int frag_index;
	//int pix_index;
	int2 pixel_coords;
	int2 ifrag_coords;
	float2 ffrag_coords;
	float2 ffrag_newcrd;
	float2 screen_coords;
	double s_dist, s_rand;
	float s_prad, d_frac;
	
	sprc = VectRot(star.position - render_info.cam_pos, render_info.cam_ori);
	
	if (sprc.z > 0.0f) {
		screen_coords.x = render_info.half_X + (sprc.x / sprc.z) * render_info.cam_foc;
		screen_coords.y = render_info.half_Y + (sprc.y / sprc.z) * render_info.cam_foc;
		
		pixel_coords.x = screen_coords.x;
		pixel_coords.y = screen_coords.y;

		if (pixel_coords.x < render_info.pixels_X && pixel_coords.y < render_info.pixels_Y) {

			s_dist = VectMag(sprc);
			d_frac = fmin((float)(s_dist / SECTOR_HALF), 1.0f);
			s_prad = StarGlowSize(star.luminosity, s_dist);
			
			bcol.rgba = (uchar4)(255,255,255, round(d_frac*255.0));
			color = ScaleColor(AlphaBlend(star.color, bcol), 1.0f-d_frac);
		
			if (s_prad < 0.5f) {
			
				ffrag_coords.x = screen_coords.x - pixel_coords.x;
				ffrag_coords.y = screen_coords.y - pixel_coords.y;
				ifrag_coords.x = ffrag_coords.x * ALMOST_TWO;
				ifrag_coords.y = ffrag_coords.y * ALMOST_TWO;
				color = ScaleColor(color, s_prad+1.0f);
				frag_buffer[(pixel_coords.y * render_info.pixels_X) + pixel_coords.x].
				colors[(ifrag_coords.y * render_info.aa_dim) + ifrag_coords.x] = color;

				// chance to fill nearby fragments to create twinkling effect for distant stars
				s_rand = cos(render_info.d_time*1e-4) * sin((double)RandomLong(zone_index&render_info.rand_int));
		
				if (s_rand > 1.0-(d_frac*s_prad)) {
					screen_coords += render_info.aa_div;
					ffrag_newcrd.x = screen_coords.x-(int)screen_coords.x;
					ffrag_newcrd.y = screen_coords.y-(int)screen_coords.y;
					if ((ffrag_coords.x != ffrag_newcrd.x) && (int)screen_coords.x < render_info.pixels_X) {
						frag_buffer[((int)screen_coords.y * render_info.pixels_X) + (int)screen_coords.x].
						colors[(ifrag_coords.y * render_info.aa_dim) + (int)(ffrag_newcrd.x * ALMOST_TWO)] = color;
					} else if ((ffrag_coords.y != ffrag_newcrd.y) && (int)screen_coords.y < render_info.pixels_Y) {
						frag_buffer[((int)screen_coords.y * render_info.pixels_X) + (int)screen_coords.x].
						colors[((int)(ffrag_newcrd.y * ALMOST_TWO) * render_info.aa_dim) + ifrag_coords.x] = color;
					}
					
					screen_coords -= render_info.aa_inc;
					ffrag_newcrd.x = screen_coords.x-(int)screen_coords.x;
					ffrag_newcrd.y = screen_coords.y-(int)screen_coords.y;
					if ((ffrag_coords.x != ffrag_newcrd.x) && (int)screen_coords.x > 0) {
						frag_buffer[((int)screen_coords.y * render_info.pixels_X) + (int)screen_coords.x].
						colors[(ifrag_coords.y * render_info.aa_dim) + (int)(ffrag_newcrd.x * ALMOST_TWO)] = color;
					} else if ((ffrag_coords.y != ffrag_newcrd.y) && (int)screen_coords.y > 0) {
						frag_buffer[((int)screen_coords.y * render_info.pixels_X) + (int)screen_coords.x].
						colors[((int)(ffrag_newcrd.y * ALMOST_TWO) * render_info.aa_dim) + ifrag_coords.x] = color;
					}
				}
				
			} else {
				// create depth index [0-9]
				uint depth_index = floor(d_frac*9.9999f);
				// create instance data structure
				InstanceData idata = {
					(float4)(ColorToVec3(color), 1.0f),
					(float2)(screen_coords.x, screen_coords.y),
					s_prad * STAR_TEX_RATIO, d_frac
				};
				// save instance data to buffer (into 10 groups separated by depth)
				idata_buffer[increment(&adb_sizes[depth_index])+(IBUFF_OFFSET*depth_index)] = idata;
				
				// should save index of closest star
				if (d_frac < FLARE_VIS_DEPTH) {
					*loaded_star = zone_index;
				}
			}
		}
	}
}

__kernel void MousePickPlanet(__global Planet* planet_buffer, __global Moon* moon_buffer,
__global volatile uint* min_dist, __global int* planet_found, __global float2* mouse_pos, const RenderInfo render_info)
{
	uint planet_index = get_global_id(0);
	Planet planet = planet_buffer[planet_index];

	double3 mouseRay = render_info.bl_ray + (render_info.cam_rgt * mouse_pos->x);
	mouseRay = normalize(mouseRay + (render_info.cam_up * mouse_pos->y));
	
	float fakeRad = planet.radius * PLANET_MAG;
	double sDist = RaySphereIntersect(render_info.cam_pos, mouseRay, planet.position, fakeRad*fakeRad);
	uint fltInt;
	
	if (sDist > 0.0) {
		fltInt = sDist * 0.0001;
		atomic_min(min_dist, fltInt);
		barrier(CLK_GLOBAL_MEM_FENCE);
		if (*min_dist == fltInt) {
			planet_found[0] = planet_index;
		}
		return;
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	uint moon_index;
	uint mstart = planet_index * MAX_MOONS;
	
	for (uint m=0; m < planet.moons; ++m) {
	
		moon_index = mstart + m;
		Moon moon = moon_buffer[moon_index];
		
		fakeRad = moon.radius * PLANET_MAG;
		sDist = RaySphereIntersect(render_info.cam_pos, mouseRay, moon.position, fakeRad*fakeRad);
		
		if (sDist > 0.0) {
			fltInt = sDist * 0.0001;
			atomic_min(min_dist, fltInt);
			if (*min_dist == fltInt) {
				planet_found[1] = moon_index;
			}
		}
	}
}

__kernel void MousePickStar(__global Star* star_buffer, __global volatile uint* min_dist,
__global int* star_found, __global float2* mouse_pos, const RenderInfo render_info)
{
    uint zone_X = get_global_id(0);
	uint zone_Y = get_global_id(1);
	uint zone_Z = get_global_id(2);
	uint zone_index = zone_X + SECTOR_SPAN * (zone_Y + SECTOR_SPAN * zone_Z);
	
	Star star = star_buffer[zone_index];
	double3 sprc;
	float2 screen_coords;
	float s_prad, p_dist;
	uint fltInt;
	
	sprc = VectRot(star.position - render_info.cam_pos, render_info.cam_ori);
	
	if (sprc.z > 0.0f) {
		screen_coords.x = render_info.half_X + (sprc.x / sprc.z) * render_info.cam_foc;
		screen_coords.y = render_info.half_Y + (sprc.y / sprc.z) * render_info.cam_foc;
		
		p_dist = Dist2D(*mouse_pos, screen_coords);
		s_prad = StarGlowSize(star.luminosity, VectMag(sprc));
		fltInt = s_prad * 10000.0f;

		if (p_dist < s_prad+5.0f) {
			atomic_min(min_dist, fltInt);
			barrier(CLK_GLOBAL_MEM_FENCE);
			if (*min_dist == fltInt) {
				*star_found = zone_index;
			}
		}
	}
}

__kernel void FillFragBuff(__global PFrags* frag_buffer, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	uint pix_seed = render_info.rand_int * pix_index;

	ulong xseed = RandomLong(pix_seed * pix_X);
	ulong yseed = RandomLong(pix_seed * pix_Y);
	uchar noiseMag = (xseed & yseed) % NOISE_MAX;
	RGB32 noiseClr = {noiseMag, noiseMag, noiseMag, 0};
	
	frag_buffer[pix_index].data = (uint4)(noiseClr.data,noiseClr.data,noiseClr.data,noiseClr.data);
}

__kernel void FillFragBuffRT(__global PFrags* frag_buffer, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	
	frag_buffer[(pix_Y * render_info.pixels_X) + pix_X].data = (uint4)(0,0,0,0);
}

__kernel void FragsToFrame(__global PFrags* frag_buffer,
write_only image2d_t pix_buffer, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	
	PFrags frags = frag_buffer[pix_index];
	float3 sumColor = (float3)(0.0f,0.0f,0.0f);
		
	for (uint r=0; r < render_info.aa_lvl; ++r) {
		sumColor.x += frags.colors[r].red;
		sumColor.y += frags.colors[r].green;
		sumColor.z += frags.colors[r].blue;
	}
	
	write_imagef(pix_buffer, (int2)(pix_X, pix_Y), NormalizeColor(sumColor * render_info.aa_div, 1.0f));
}

__kernel void FragsToFrameRT(__global PFrags* frag_buffer,
write_only image2d_t pix_buffer, const RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	
	PFrags frags = frag_buffer[pix_index];
	float3 sumColor = (float3)(0.0f,0.0f,0.0f);
	float ccount = 0.0f;
		
	for (uint r=0; r < render_info.aa_lvl; ++r) {
		if (frags.colors[r].alpha > 0) {
			sumColor.x += frags.colors[r].red;
			sumColor.y += frags.colors[r].green;
			sumColor.z += frags.colors[r].blue;
			ccount += 1.0f;
		}
	}
	
	if (ccount == 0.0f) {
		write_imagef(pix_buffer, (int2)(pix_X, pix_Y), (float4)(0.0f,0.0f,0.0f,0.0f));
	} else {
		write_imagef(pix_buffer, (int2)(pix_X, pix_Y), NormalizeColor(sumColor/ccount, ccount*render_info.aa_div));
	}
}