#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include "Data/textures.inc"
#include "Data/strings.inc"
#include "Data/sounds.inc"

#define CAMTILT_SPEED	0.001
#define CAMLOCK_SPEED	0.001
#define GUISLIDE_SPEED  3.0f

#define SECTOR_SPAN     32
#define GALAXY_SPAN     2642245
#define ZONE_WIDTH      4.73e16
#define ZONE_HALF       2.365e16
#define LIGHT_YEAR      9.4607304725808e15
#define SECTOR_HALF     7.568e17
#define ONE_AU          149597870700.0
#define ATAN2_1_0       1.57079632679489
#define FLARE_ADM       1000.0f
#define FLARE_VIS_DEPTH 0.001f
#define STAR_VIS_DEPTH  0.0001f
#define STAR_TEX_RATIO  4.0f
#define ISL_DIST_MULT   0.00000005
#define PLANET_MAG      1000.0f
#define STAR_MAG        10.0f
#define STAR_MAX_TEMP   9.188e26
#define SCAN_TIME_MS    3000.0

#define EARTH_AGE       4543.0f
#define EARTH_MASS      5.972e24
#define EARTH_DENS      5513.58f
#define EARTH_RADI      6.3781e6
#define EARTH_TEMP      287.75f

#define SOLAR_MASS      2e30
#define SOLAR_RADI      69551e4
#define SOLAR_TEMP      5778.0f
#define SOLAR_LUMI      3.828e26

#define MAX_PLANETS     10
#define MAX_MOONS       4

#define IBUFFER_OFFSET  999
#define IBUFFER_SIZE    10000

#define AA_SUB_RAYS     4

#define CL_LOGGING		1
#define CL_COMPLOG		1
#define GL_DEBUG        0
#define CL_DEBUG        0

#define FONT_CONTHRAX   0
#define FONT_OXANIUM    1
#define FONT_SULPHURP   2
#define FONT_XOLONIUM   3
#define FONT_ZEKTON     4

#define CONFIG_FILE     "settings.cfg"
#define CL_BUILD_LOG    "logs/cl_build.log"

#define WINDOW_TITLE	"Cosmic Void"

#if defined (__APPLE__) || defined(MACOSX)
	#define CL_GL_SHARING_EXT "cl_APPLE_gl_sharing"
#else
	#define CL_GL_SHARING_EXT "cl_khr_gl_sharing"
#endif

#define KEY_MAP(X) GLOBALS::key_map[X]

#define LANG(X) GLOBALS::lang_vec[X].first

namespace GLOBALS {

	extern std::unordered_map<std::string,std::string> config_map;
	extern std::vector<std::pair<std::string,std::string>> lang_vec;
	extern std::array<int,99> key_map;
    extern std::string DATA_FOLDER;
    extern std::mutex COUT_MUTEX;
}

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
