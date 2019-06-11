#pragma once
#include "GLGraphics.h"
#include "CLTypes.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "Camera.h"
#include "Profiles.h"
#include "Sound.h"
#include <time.h>
#include <cstdlib>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>

class Game
{
public:
	Game(GLFWwindow* window, KeyboardServer& kServer, MouseServer& mServer);
	~Game();
	void Go();
private:
    void ApplyKeyCodes();
	void GenerateSolSystem();
	void RenderSolSystem();
	void RenderFarStars();
	void RenderNearStars();
	void RenderSprites();
	void FinishFrameCL();
	void FinishFrameRT();
	void UpdateCamera();
	void HandleInput();
	void BeginActions();
	void ComposeFrame();
	void Render2DGUI();

	//std::vector<ftgl::FreetypeGlText> glTexts;
	std::array<ftgl::Markup,5> fontStyles;

	KeyboardClient kbd;
	MouseClient mouse;
	GLGraphics gfx;
	SoundManager sound;

	CL openCL;
	cl_int cl_error;
	cl_RenderInfo rInfo;
	std::vector<cl_uint> idatSizes;

	cl::Buffer cl_systemBuff;
	cl::Buffer cl_moonBuff;
	cl::Buffer cl_planetBuff;
	cl::Buffer cl_starBuff;
	cl::Buffer cl_fragBuff;
	cl::Buffer cl_mousePosBuff;
	cl::Buffer cl_volatileBuff;
	cl::Buffer cl_idatSizesBuff;
	cl::Buffer cl_indexBuff;
	cl::BufferGL gl_idatBuff;

	std::vector<std::vector<cl_Moon>> moons;
	std::vector<cl_Planet> planets;
	std::vector<cl_InstanceData> instList;
	std::vector<std::pair<double,uint8_t>> pDistList;
	std::vector<std::pair<double,uint8_t>> mDistList;
	cl_SolarSystem solSystem;
	cl_Star loadedStar;
	cl_Star pickedStar;
	cl_Planet pickedPlanet;
	cl_Moon pickedMoon;

	//Scene scene;
	Camera camera;
	cl_AAInfo aaInfo;
	SCREEN::Resolution resolution;

	int3 globShift;
	uint3 loadedGlobPos;
	uint3 pickedGlobPos;
	float3 loadedStarClr;
	float2 loadedStarSP;
	uint64_t loadedGlobID;
	uint64_t pickedGlobID;
	int32_t sub_rays;
	int32_t pickedStarIndex;
	int32_t pickedPlanetIndex;
	uint32_t nearestIndex;
	uint32_t loadedIndex;
	uint32_t instanceIndex;
	uint32_t pixCount, fragCount;
	uint32_t heightSpan, widthSpan;
	uint32_t heightRays, widthRays;
	uint32_t heightHalf, widthHalf;
	float maxX, minX, maxY, minY;
	float objDist, dX, dY;
	float cdFrac, ctFrac;
	float lsDistFrac;
	float lsLumiFrac;
	float lockProgress;
	float scanProgress;
	double movementSpeed;
	double pickedDist;
	double loadedDist;
	bool genStars = true;
	bool isStarLoaded = false;
	bool doLoadStar = false;
	bool doLensFlare = false;
	bool doPickStar = false;
	bool doPickPlanet = false;
	bool starPicked = false;
	bool planetPicked = false;
	bool camLock = false;
    bool camLocked = false;
    bool scanLocked = false;
    bool showScanStats = false;
    bool hideNearStarSprite;
	bool exitGame, isGamePaused;

	double deltaTime;
	Timer deltaTimer;
	Timer scanTimer;

	DVec3 zonePos;
    DVec3 loadedStarPos;
    DVec3 pickedPos;

    float2 lsScreenPos;
    cl_float2 lsScreenXY;

	union {
	    cl_uint3 globXYZ;
        uint3 globPos;
	};
};
