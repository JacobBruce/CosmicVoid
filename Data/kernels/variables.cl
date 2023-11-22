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
#define ZONE_SPLIT  2.5e15
#define PI			3.141592653589793
#define PI_MUL_4o3  4.18879020478639
#define PI_MUL_4    12.5663706143591
#define PI_MUL_2    6.28318530717958
#define PI_HALF		1.57079632679489
#define ONE_AU      149597870700.0
#define TWO_AU      299195741400.0
#define G           6.674e-11

#define PLANET_TEX_W    2000
#define PLANET_TEX_H    1000
#define MOON_TEX_W    	800
#define MOON_TEX_H    	400

#define MAX_PLANETS 8
#define MAX_MOONS   4

#define EARTH_AGE  4543.0f
#define EARTH_AGED 5678.75f
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

#define DEF_ATMOS  0
#define DUST_ATMOS 1
#define LIFE_ATMOS 2
#define FIRE_ATMOS 3
#define GAS_ATMOS  4

#define RED_RGB     245,100,40
#define ORANGE_RGB  245,180,40
#define YELLOW_RGB  245,245,55
#define LYELLOW_RGB 252,252,240
#define BROWN_RGB   120,55,5
#define BLUE_RGB    40,140,245
#define LBLUE_RGB   240,245,252
#define BLACK_RGB   0,0,0

#define STAR_MAG 10.0f
#define PLANET_MAG 1000.0f
#define CLOUD_SPEED 0.005f
#define WAVE_SPEED 0.2f
#define WAVE_SCALE 25.0f
#define WAVE_DETAIL 3
#define NOISE_MAX   6
#define MOON_TEMP 100.0f
#define LAVA_MIN_TEMP 1000.0f
#define LAVA_MAX_TEMP 6000.0f
#define FREEZING_TEMP 273.15f
#define BOILING_TEMP 373.15f
#define ATMO_B_ALPHA 200.0f
#define ATMO_DIST_POW 1.75f
#define RAY_DENS_MUL 0.08f
#define RAY_STEP_DIV 6.0f
#define MAX_GRAVITY 222.0f
#define FLARE_VIS_DEPTH 0.005f
#define STAR_TEX_RATIO 4.0f
#define STAR_LIGHT_POW 0.001f
#define ISL_DIST_MULT 0.00000005
#define MAX_MOON_GAP 5e8
#define MAX_ORBIT_TILT 0.25
#define MIN_ROT_TIME 10000
#define MAX_ROT_TIME 5000000
#define IBUFF_OFFSET 999
#define MAX_MILL_SPECIES 20

// ------------------------------ //
// ------ CONSTANT GLOBALS ------ //
// ------------------------------ //

//diffuse rgb, ambient light
__constant float4 moonDiffData[4] = {
	(float4)(1.0f,1.0f,1.0f, 0.05f),
	(float4)(0.5f,0.5f,0.5f, 0.02f),
	(float4)(0.2f,0.2f,0.2f, 0.02f),
	(float4)(0.9f,0.9f,0.9f, 0.02f)
};

//specular rgb, roughness
__constant float4 moonSpecData[4] = {
	(float4)(0.1f,0.1f,0.1f, 90.0f),
	(float4)(0.7f,0.7f,0.7f, 60.0f),
	(float4)(0.5f,0.5f,0.5f, 70.0f),
	(float4)(0.2f,0.2f,0.2f, 80.0f)
};

//diffuse rgb, ambient light
__constant float4 planetDiffData[12] = {
	(float4)(0.9f,0.9f,0.9f, 0.02f),
	(float4)(1.0f,1.0f,1.0f, 0.02f),
	(float4)(1.0f,1.0f,1.0f, 0.02f),
	(float4)(0.5f,0.5f,0.5f, 0.02f),
	(float4)(0.5f,0.5f,0.5f, 0.02f),
	(float4)(0.2f,0.2f,0.2f, 0.02f),
	(float4)(0.8f,0.8f,0.8f, 0.02f),
	(float4)(0.9f,0.9f,0.9f, 0.02f),
	(float4)(0.8f,0.8f,0.8f, 0.02f),
	(float4)(1.0f,1.0f,1.0f, 0.05f),
	(float4)(1.0f,1.0f,1.0f, 0.02f),
	(float4)(0.6f,0.6f,0.6f, 0.05f)
};

//specular rgb, roughness
__constant float4 planetSpecData[12] = {
	(float4)(0.2f,0.2f,0.2f, 80.0f),
	(float4)(0.1f,0.1f,0.1f, 95.0f),
	(float4)(0.1f,0.1f,0.1f, 95.0f),
	(float4)(0.7f,0.7f,0.7f, 60.0f),
	(float4)(0.7f,0.7f,0.7f, 60.0f),
	(float4)(0.5f,0.5f,0.5f, 70.0f),
	(float4)(0.3f,0.3f,0.3f, 85.0f),
	(float4)(0.1f,0.1f,0.1f, 90.0f),
	(float4)(0.2f,0.2f,0.2f, 80.0f),
	(float4)(0.05f,0.05f,0.05f, 90.0f),
	(float4)(0.01f,0.01f,0.01f, 99.0f),
	(float4)(0.4f,0.4f,0.4f, 75.0f)
};

//rgb color, base density
__constant uchar4 atmosphereColors[5] = {
	(uchar4)(216, 240, 245, 10), //DEFAULT
	(uchar4)(237, 237, 237, 20), //DUST
	(uchar4)(147, 211, 245, 30), //LIFE
	(uchar4)(235, 186, 136, 40), //FIRE
	(uchar4)(65,  174, 224, 50)  //GAS
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
	(float4)(0.8,0.4,   0.2,0.1),//Dwarf planet 0
	(float4)(0.3,2.8,   0.15,1.1),//Gas dwarf 1
	(float4)(0.3,8.0,   0.15,4.0),//Gas giant 2
	(float4)(0.8,2.0,   0.3,1.5),//Ice planet 3
	(float4)(0.9,4.0,   0.2,1.0),//Ice giant 4
	(float4)(0.6,1.5,   0.2,1.0),//Water planet 5
	(float4)(1.0,1.0,   0.2,0.5),//Terrestrial planet 6 [Earth-like planet 6 , Desert planet 7, Silicate planet 8]
	(float4)(1.3,0.9,   0.2,0.4)//Theoretical planet 7 [Lava planet 9, Carbon planet 10, Iron planet 11]
};

//density, radius, variances
__constant float4 moonTypeStats[4] = {
	(float4)(0.7,0.3,   0.1,0.1),//Volcanic moon 0
	(float4)(0.6,0.25,  0.1,0.1),//Ice moon 1
	(float4)(0.5,0.3,   0.1,0.1),//Water moon 2
	(float4)(0.7,0.25,  0.1,0.1)//Rock moon 3
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

__constant int perm[257] = {151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151
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
	float water;
	uchar score;
	RGB32 color;
} MoonData;

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
	double g_time;
	uint3 zone_offset;
	uint rand_int;
	ushort pixels_X;
	ushort pixels_Y;
	ushort half_X;
	ushort half_Y;
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
	float rot_time;
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
	float2 orbit;
	float mass;
	float radius;
	float density;
	float gravity;
	float area;
	float temp;
	float water_frac;
	float rot_time;
	float orbit_time;
	float orbit_rad;
	float eccentricity;
	float velocity;
	float atmos_radi;
	RGB32 atmos_spec;
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
	float2 orbit;
	float mass;
	float radius;
	float density;
	float gravity;
	float area;
	float temp;
	float water_frac;
	float rot_time;
	float orbit_time;
	float orbit_rad;
	float eccentricity;
	float velocity;
	float atmos_radi;
	RGB32 atmos_spec;
} Moon;

typedef struct {
	double2 plane;
	ulong index;
	uint conflict;
	uint economy;
	uint security;
	uint civ_count;
	uint moons;
	uint planets;
	uint stations;
	uint artifacts; //abandoned/destroyed ships, stations, planets
	uint anomalies; //workmholes, dyson spheres, artificial planets
	uint secrets; //alien space objects/structures
} SolarSystem;

#pragma pack(pop)
