#include "Game.h"

Game::Game(GLFWwindow* window, KeyboardServer& kServer, MouseServer& mServer)
:
	kbd( kServer ),
	mouse( mServer ),
	exitGame( false ),
	isGamePaused( false )
{
	// ensure the compiler is playing nice
	assert(sizeof(RGB32) == sizeof(cl_RGB32) && sizeof(RGB32) == 4);
	assert(sizeof(Vec3) == sizeof(cl_float3) && sizeof(Vec3) == 16);
	assert(sizeof(DVec3) == sizeof(cl_double3) && sizeof(DVec3) == 32);
	//assert(sizeof(Material) == sizeof(cl_Material) && sizeof(Material) == 64);
	//assert(sizeof(cl_RayIntersect) == 32);
	assert(sizeof(cl_Star) == 64);
	assert(sizeof(cl_Planet) == 96);
	assert(sizeof(cl_Moon) == 96);
	assert(sizeof(cl_SolarSystem) == 64);
	assert(sizeof(cl_SpriteData) == 36);
	assert(sizeof(cl_AnimSpriteData) == 40);
	assert(sizeof(cl_InstanceData) == 32);
	assert(sizeof(cl_RenderInfo) == 256);

	//trans_depth = stoi(GLOBALS::config_map["TRANS_DEPTH"]);
	sub_rays = AA_SUB_RAYS;//stoi(GLOBALS::config_map["AA_SUB_RAYS"]);

	switch (sub_rays) {
		case 1: aaInfo = GLOBALS::AA_X1; break;
		case 4: aaInfo = GLOBALS::AA_X4; break;
		case 9: aaInfo = GLOBALS::AA_X9; break;
		case 16: aaInfo = GLOBALS::AA_X16; break;
		default:
			HandleFatalError(1, "Invalid pixel ray count: "+VarToStr(sub_rays));
			break;
	}

	// Set key codes from config
    ApplyKeyCodes();

	// Initialize OpenCL
	openCL.Initialize();

	// Initialize graphics manager
	gfx.Initialize(window, openCL);

	// Load texture files
	gfx.LoadTextures("./Data/textures/textures.list");

	// Load font files
	gfx.LoadFonts("./Data/fonts/fonts.list", "./Data/fonts/font_chars.list");

	fontStyles[FONT_CONTHRAX] = std::move(gfx.openGL.CreateMarkup("Conthrax", gfx.glFonts[FONT_CONTHRAX]));
	fontStyles[FONT_OXANIUM] = std::move(gfx.openGL.CreateMarkup("Oxanium", gfx.glFonts[FONT_OXANIUM]));
	fontStyles[FONT_SULPHURP] = std::move(gfx.openGL.CreateMarkup("SulphurPoint", gfx.glFonts[FONT_SULPHURP]));
	fontStyles[FONT_XOLONIUM] = std::move(gfx.openGL.CreateMarkup("Xolonium", gfx.glFonts[FONT_XOLONIUM]));
	fontStyles[FONT_ZEKTON] = std::move(gfx.openGL.CreateMarkup("Zekton", gfx.glFonts[FONT_ZEKTON]));

	// Load wav audio files
	sound.LoadSounds("./Data/audio/audio.list");

	// Load material properties library
	//matSet.Load("Data\\materials.list");

	// Get window resolution profile
    resolution = SCREEN::GetProfile(gfx.windowWidth, gfx.windowHeight);

	// set camera sensitivity based on settings
	camera.sensitivity = stof(GLOBALS::config_map["MOUSE_SENSI"]);
	movementSpeed = 4.73e13;

	srand(time(NULL));
	globPos.x = 1321122;//rand() % GALAXY_SPAN;
	globPos.y = 1321122;//rand() % GALAXY_SPAN;
	globPos.z = 1321122;//rand() % GALAXY_SPAN;
	zonePos.x = 0.0;//((double(rand()) / RAND_MAX) * 2.0) - 1.0;
	zonePos.y = 0.0;//((double(rand()) / RAND_MAX) * 2.0) - 1.0;
	zonePos.z = 0.0;//((double(rand()) / RAND_MAX) * 2.0) - 1.0;
	camera.position.x = zonePos.x * ZONE_HALF;
	camera.position.y = zonePos.y * ZONE_HALF;
	camera.position.z = zonePos.z * ZONE_HALF;

	// calc useful screen info
	widthHalf = gfx.windowWidth / 2;
	heightHalf = gfx.windowHeight / 2;
	widthRays = gfx.windowWidth * aaInfo.lvl;
	heightRays = gfx.windowHeight * aaInfo.lvl;
	heightSpan = gfx.windowHeight - 1;
	widthSpan = gfx.windowWidth - 1;
	pixCount = gfx.windowWidth * gfx.windowHeight;
	fragCount = pixCount * aaInfo.lvl;

	rInfo.zone_offset = globXYZ;
	rInfo.aa_info = aaInfo;
	rInfo.pixels_X = gfx.windowWidth;
	rInfo.pixels_Y = gfx.windowHeight;
	rInfo.half_X = widthHalf;
	rInfo.half_Y = heightHalf;

	instList.reserve(99);
	idatSizes.resize(10);

	moons.resize(MAX_MOONS*MAX_PLANETS);
	planets.resize(MAX_PLANETS);
	pDistList.resize(MAX_PLANETS);
	mDistList.resize(MAX_MOONS);

	// create buffer for OCL from OGL instance buffer
	gl_idatBuff = cl::BufferGL(openCL.context, CL_MEM_READ_WRITE, gfx.openGL.StarBuffer());

	// allocate memory on GPU for pixel fragment buffer
	cl_fragBuff = cl::Buffer(openCL.context, CL_MEM_WRITE_ONLY, sizeof(cl_RGB32)*fragCount);

	// allocate memory on GPU for system buffer
	cl_systemBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_SolarSystem));

	// allocate memory on GPU for moon buffer
	cl_moonBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_Moon)*MAX_MOONS*MAX_PLANETS);

	// allocate memory on GPU for planet buffer
	cl_planetBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_Planet)*MAX_PLANETS);

	// allocate memory on GPU for star buffer
	cl_starBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_Star)*std::pow(SECTOR_SPAN,3));

	// allocate memory on GPU for intersection buffer (use to draw depth tex, deferred ray shading, etc)
	//cl_intersectBuff = cl::Buffer(openCL.context, CL_MEM_WRITE_ONLY, sizeof(cl_RayIntersect)*fragCount);

	// allocate memory on GPU for write-only volatile buffer
	cl_volatileBuff = cl::Buffer(openCL.context, CL_MEM_WRITE_ONLY, sizeof(cl_uint));

	// allocate memory on GPU for atomic instance counters
	cl_idatSizesBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_uint)*10);

	// allocate memory on GPU for loaded star index buffer
	cl_indexBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_uint)*10);

	// allocate memory on GPU for mouse position buffer
	cl_mousePosBuff = cl::Buffer(openCL.context, CL_MEM_READ_WRITE, sizeof(cl_float2));

	// generate stars for surrounding zones
	openCL.GS_Kernel.setArg(0, cl_starBuff);
	openCL.GS_Kernel.setArg(1, rInfo);
	openCL.GenStars();
	openCL.queue.finish();
}


Game::~Game(){}

void Game::Go()
{
	deltaTime = deltaTimer.MilliCount();
	deltaTimer.ResetTimer();
	gfx.BeginFrame();
	ComposeFrame();
	gfx.DisplayFrame();

	if (exitGame)
        glfwSetWindowShouldClose(gfx.window, GLFW_TRUE);
}

void Game::ApplyKeyCodes()
{
    GLOBALS::key_map[KEY_MOVE_UP] = stoi(GLOBALS::config_map["KEY_MOVE_UP"]);
    GLOBALS::key_map[KEY_MOVE_DOWN] = stoi(GLOBALS::config_map["KEY_MOVE_DOWN"]);
    GLOBALS::key_map[KEY_MOVE_FWRD] = stoi(GLOBALS::config_map["KEY_MOVE_FWRD"]);
    GLOBALS::key_map[KEY_MOVE_BACK] = stoi(GLOBALS::config_map["KEY_MOVE_BACK"]);
    GLOBALS::key_map[KEY_MOVE_LEFT] = stoi(GLOBALS::config_map["KEY_MOVE_LEFT"]);
    GLOBALS::key_map[KEY_MOVE_RIGHT] = stoi(GLOBALS::config_map["KEY_MOVE_RIGHT"]);
    GLOBALS::key_map[KEY_MOVE_TILTL] = stoi(GLOBALS::config_map["KEY_MOVE_TILTL"]);
    GLOBALS::key_map[KEY_MOVE_TILTR] = stoi(GLOBALS::config_map["KEY_MOVE_TILTR"]);
    GLOBALS::key_map[KEY_TARG_LOCK] = stoi(GLOBALS::config_map["KEY_TARG_LOCK"]);
    GLOBALS::key_map[KEY_TARG_SCAN] = stoi(GLOBALS::config_map["KEY_TARG_SCAN"]);
    GLOBALS::key_map[KEY_AUTO_PILOT] = stoi(GLOBALS::config_map["KEY_AUTO_PILOT"]);
}

void Game::HandleInput()
{
	// key for exiting game
	if (kbd.KeyIsPressed(GLFW_KEY_ESCAPE)) exitGame = true;

	// keys for left and right tilt
	if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_TILTR))) {
		camera.orientation.z -= CAMTILT_SPEED * deltaTime;
	} else if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_TILTL))) {
		camera.orientation.z += CAMTILT_SPEED * deltaTime;
	}

	// keys for up and down movement
	if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_UP))) {
		camera.position += camera.up * (movementSpeed * deltaTime);
        camLocked = false;
        planetPicked = false;
	} else if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_DOWN))) {
		camera.position -= camera.up * (movementSpeed * deltaTime);
        camLocked = false;
        planetPicked = false;
	}

	// keys for forward and backward movement
	if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_FWRD))) {
		camera.position += camera.forward * (movementSpeed * deltaTime);
	} else if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_BACK))) {
		camera.position -= camera.forward * (movementSpeed * deltaTime);
	}

	// keys for right and left movement
	if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_RIGHT))) {
		camera.position += camera.right * (movementSpeed * deltaTime);
        camLocked = false;
        planetPicked = false;
	} else if (kbd.KeyIsPressed(KEY_MAP(KEY_MOVE_LEFT))) {
		camera.position -= camera.right * (movementSpeed * deltaTime);
        camLocked = false;
        planetPicked = false;
	}

	// keys for camera speed adjustment
	if (kbd.KeyIsPressed(GLFW_KEY_PAGE_UP)) {
		movementSpeed += std::min(movementSpeed * 1.01, 1e12);
	} else if (kbd.KeyIsPressed(GLFW_KEY_PAGE_DOWN)) {
		movementSpeed -= movementSpeed * 0.01;
	}

	KeyEvent ke = kbd.ReadKey();
	if (ke.IsPress()) {
		switch (ke.GetCode()) {
		case GLFW_KEY_SCROLL_LOCK:
			gfx.ToggleCursorLock();
			break;
		default: break;
		}
	} else if (ke.IsRelease()) {
        // key for locking onto objects
        if (KEY_MAP(KEY_TARG_LOCK) == ke.GetCode()) {
            if (starPicked || planetPicked) {
                camLock = true;
                camLocked = false;
                scanLocked = false;
                showScanStats = false;
                sound.ForcePlay(Sounds::LOCKING_TARGET);
            }
        // key for analyzing locked object
        } else if (KEY_MAP(KEY_TARG_SCAN) == ke.GetCode()) {
            if (planetPicked && camLocked) {
                scanTimer.ResetTimer();
                scanLocked = true;
                showScanStats = false;
                sound.ForcePlay(Sounds::ANALYZING_PLANET);
            }
        }
	}

	// handle mouse events
	MouseEvent me = mouse.ReadMouse();
	switch (me.GetType()) {
	case MouseEvent::Move:
		if (gfx.cursorLocked) {
			dX = me.GetX() * camera.sensitivity * deltaTime;
			dY = me.GetY() * camera.sensitivity * deltaTime;
			camera.orientation.x += dY;
			camera.orientation.y -= dX;
			glfwSetCursorPos(gfx.window, 0.0, 0.0);
			camLock = false;
			camLocked = false;
            planetPicked = false;
		}
		break;
	case MouseEvent::WheelUp:
		camera.foclen += 10;
		break;
	case MouseEvent::WheelDown:
		camera.foclen -= 10;
		break;
	case MouseEvent::LPress:
	    if (!isGamePaused) {
            if (isStarLoaded) {
                doPickPlanet = true;
            } else {
                doPickStar = true;
            }
            camLock = false;
            camLocked = false;
            starPicked = false;
            planetPicked = false;
            showScanStats = false;
	    }
		break;
	case MouseEvent::RPress:
		break;
	case MouseEvent::LRelease:
		break;
	case MouseEvent::RRelease:
		break;
	case MouseEvent::Invalid:
		break;
	}
}

void Game::UpdateCamera()
{
    static double xz_angle, yz_angle;
    static double xza_abs, yza_abs;
	static double xza_start = 0.0;
	static double yza_start = 0.0;
    // move camera towards locked object
    if (camLock && (starPicked || planetPicked)) {

        xz_angle = std::atan2(pickedPos.z,pickedPos.x) - ATAN2_1_0;
        yz_angle = std::atan2(pickedPos.z,pickedPos.y) - ATAN2_1_0;
        xza_abs = std::abs(xz_angle);
        yza_abs = std::abs(yz_angle);

        if (xza_start == 0.0 && yza_start == 0.0) {
            xza_start = std::abs(xz_angle);
            yza_start = std::abs(yz_angle);
        }

        lockProgress = std::min(2.0f, float((xza_abs / xza_start) + (yza_abs / yza_start)));

        if (xza_abs < 0.0015 && yza_abs < 0.0015) {
            camera.orientation.x += yz_angle;
            camera.orientation.y += xz_angle;
            xza_start = 0.0;
            yza_start = 0.0;
            camLock = false;
            camLocked = true;
            sound.ForcePlay(Sounds::LOCK_ACQUIRED);
        } else {
            if (yz_angle > 0.0f) {
                camera.orientation.x += std::min(yz_angle, std::max(0.001, yz_angle * CAMLOCK_SPEED * deltaTime));
            } else {
                camera.orientation.x += std::max(yz_angle, std::min(-0.001, yz_angle * CAMLOCK_SPEED * deltaTime));
            }
            if (xz_angle > 0.0f) {
                camera.orientation.y += std::min(xz_angle, std::max(0.001, xz_angle * CAMLOCK_SPEED * deltaTime));
            } else {
                camera.orientation.y += std::max(xz_angle, std::min(-0.001, xz_angle * CAMLOCK_SPEED * deltaTime));
            }
        }
    }

	// update camera direction
	camera.UpdateDirection();
}

void Game::BeginActions()
{
	// create ray through bottom left of virtual screen
	camera.bl_ray = (camera.forward * camera.foclen).
					VectSub(camera.right * (widthHalf-aaInfo.div)).
					VectSub(camera.up * (heightHalf-aaInfo.div));

	// save data to RenderInfo structure
	rInfo.cam_info = camera.GetInfo();
	rInfo.rand_int = rand();
	rInfo.delta_time = deltaTime;

	zonePos = camera.position / ZONE_HALF;
	globShift = {0,0,0};
	genStars = false;

	if (zonePos.x > 1.0) {
		globShift.x = -1;
		zonePos.x = -1.0 + (zonePos.x - 1.0);
		genStars = true;
	} else if (zonePos.x < -1.0) {
		globShift.x = 1;
		zonePos.x = 1.0 + (zonePos.x + 1.0);
		genStars = true;
	}

	if (zonePos.y > 1.0) {
		globShift.y = -1;
		zonePos.y = -1.0 + (zonePos.y - 1.0);
		genStars = true;
	} else if (zonePos.y < -1.0) {
		globShift.y = 1;
		zonePos.y = 1.0 + (zonePos.y + 1.0);
		genStars = true;
	}

	if (zonePos.z > 1.0) {
		globShift.z = -1;
		zonePos.z = -1.0 + (zonePos.z - 1.0);
		genStars = true;
	} else if (zonePos.z < -1.0) {
		globShift.z = 1;
		zonePos.z = 1.0 + (zonePos.z + 1.0);
		genStars = true;
	}

	globPos.x += globShift.x;
	globPos.y += globShift.y;
	globPos.z += globShift.z;

	openCL.FB_Kernel.setArg(0, cl_fragBuff);
	openCL.FB_Kernel.setArg(1, rInfo);
	openCL.FillFragBuff(gfx.windowWidth, gfx.windowHeight);
	openCL.queue.finish();
}

void Game::RenderFarStars()
{
	if (genStars) {
		/*std::cout << "GPX: " << gal_pos.x << std::endl;
		std::cout << "GPY: " << gal_pos.y << std::endl;
		std::cout << "GPZ: " << gal_pos.z << std::endl;
		std::cout << "ZPX: " << zonePos.x << std::endl;
		std::cout << "ZPY: " << zonePos.y << std::endl;
		std::cout << "ZPZ: " << zonePos.z << std::endl;*/

		camera.position = zonePos * ZONE_HALF;
		rInfo.cam_info.cam_pos = camera.position.vector;
		rInfo.zone_offset = globXYZ;

		openCL.GS_Kernel.setArg(0, cl_starBuff);
		openCL.GS_Kernel.setArg(1, rInfo);

		openCL.GenStars();
		openCL.queue.finish();

		if (starPicked) {

            pickedStar.position.s[0] += globShift.x * ZONE_WIDTH;
            pickedStar.position.s[1] += globShift.y * ZONE_WIDTH;
            pickedStar.position.s[2] += globShift.z * ZONE_WIDTH;

            if (pickedStar.position.s[0] > SECTOR_HALF ||
                pickedStar.position.s[1] > SECTOR_HALF ||
                pickedStar.position.s[2] > SECTOR_HALF) {

                starPicked = false;
                camLocked = false;
            }
		}
	}

    std::fill(idatSizes.begin(), idatSizes.end(), 0);

    openCL.queue.enqueueWriteBuffer(cl_idatSizesBuff, CL_TRUE, 0, sizeof(cl_uint)*10, idatSizes.data());
	openCL.queue.finish();

	openCL.DG_Kernel.setArg(0, cl_starBuff);
	openCL.DG_Kernel.setArg(1, cl_fragBuff);
	openCL.DG_Kernel.setArg(2, gl_idatBuff);
	openCL.DG_Kernel.setArg(3, cl_idatSizesBuff);
	openCL.DG_Kernel.setArg(4, cl_indexBuff);
	openCL.DG_Kernel.setArg(5, rInfo);

	openCL.DrawStars();
	openCL.queue.finish();

	openCL.queue.enqueueReadBuffer(cl_idatSizesBuff, CL_TRUE, 0, sizeof(cl_uint)*10, idatSizes.data());
	openCL.queue.finish();

	instList.resize(idatSizes[0]);

	hideNearStarSprite = false;
    doLoadStar = false;
    doLensFlare = false;
    nearestIndex = 0;
    instanceIndex = 0;

    // properly sort closest stars to avoid blending issues
    if (instList.size() > 1) {

        openCL.queue.enqueueReadBuffer(gl_idatBuff, CL_TRUE, 0, sizeof(cl_InstanceData)*instList.size(), instList.data());
        openCL.queue.finish();

        std::sort(instList.begin(), instList.end(), [](cl_InstanceData a, cl_InstanceData b){ return a.depth > b.depth; });

        if (instList.back().depth < STAR_VIS_DEPTH) {
            doLoadStar = true;
            doLensFlare = true;
            hideNearStarSprite = true;
            instanceIndex = instList.size() - 1;
        } else if (instList.back().depth < FLARE_VIS_DEPTH) {
            doLensFlare = true;
            openCL.queue.enqueueReadBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(cl_uint), &nearestIndex);
            openCL.queue.finish();
            openCL.queue.enqueueReadBuffer(cl_starBuff, CL_TRUE, sizeof(cl_Star)*nearestIndex, sizeof(cl_Star), &loadedStar);
            openCL.queue.finish();
        }

        openCL.queue.enqueueWriteBuffer(gl_idatBuff, CL_TRUE, 0, sizeof(cl_InstanceData)*instList.size(), instList.data());
        openCL.queue.finish();

    } else if (instList.size() == 1) {

        openCL.queue.enqueueReadBuffer(gl_idatBuff, CL_TRUE, 0, sizeof(cl_InstanceData), instList.data());
        openCL.queue.finish();

        if (instList[0].depth < STAR_VIS_DEPTH) {
            doLoadStar = true;
            doLensFlare = true;
            hideNearStarSprite = true;
            instanceIndex = instList.size() - 1;
        } else if (instList[0].depth < FLARE_VIS_DEPTH) {
            doLensFlare = true;
            openCL.queue.enqueueReadBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(cl_uint), &nearestIndex);
            openCL.queue.finish();
            openCL.queue.enqueueReadBuffer(cl_starBuff, CL_TRUE, sizeof(cl_Star)*nearestIndex, sizeof(cl_Star), &loadedStar);
            openCL.queue.finish();
        }
    }
}

void Game::GenerateSolSystem()
{
    if (doLoadStar) {
        if (!isStarLoaded) {

            openCL.queue.enqueueReadBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(cl_uint), &loadedIndex);
            openCL.queue.finish();
            openCL.queue.enqueueReadBuffer(cl_starBuff, CL_TRUE, sizeof(cl_Star)*loadedIndex, sizeof(cl_Star), &loadedStar);
            openCL.queue.finish();

            loadedStarPos = DVec3(loadedStar.position);
            loadedStarClr = {loadedStar.color.red/255.f, loadedStar.color.green/255.f, loadedStar.color.blue/255.f};
            ctFrac = 0.5f - std::min((loadedStar.temp / (float)STAR_MAX_TEMP) * 0.5f, 0.5f);
            lsLumiFrac = std::max(0.5f, 3.0f - std::min(3.0f, loadedStar.luminosity / (float)SOLAR_LUMI));

            loadedGlobPos = Index1Dto3D(loadedIndex, SECTOR_SPAN);
            loadedGlobPos.x += globPos.x;
            loadedGlobPos.y += globPos.y;
            loadedGlobPos.z += globPos.z;
            loadedGlobID = Index3Dto1D_64(loadedGlobPos, GALAXY_SPAN);

            uint64_t rnd1 = RandomLong(loadedGlobID);
            uint64_t rnd2 = RandomLong(rnd1);

            solSystem.planets = rnd1 % (MAX_PLANETS+1);
            solSystem.plane = DVec3::VectRnd(rnd2).vector;
            solSystem.index = loadedGlobID;
            solSystem.security = 0;
            solSystem.conflict = 0;
            solSystem.economy = 0;
            solSystem.civ_count = 0;
            solSystem.moons = 0;

            if (solSystem.planets > 0) {
                openCL.queue.enqueueWriteBuffer(cl_systemBuff, CL_TRUE, 0, sizeof(cl_SolarSystem), &solSystem);
                openCL.queue.finish();

                openCL.GP_Kernel.setArg(0, cl_systemBuff);
                openCL.GP_Kernel.setArg(1, cl_planetBuff);
                openCL.GP_Kernel.setArg(2, cl_moonBuff);
                openCL.GP_Kernel.setArg(3, cl_starBuff);
                openCL.GP_Kernel.setArg(4, cl_indexBuff);

                openCL.GenPlanets(solSystem.planets);
                openCL.queue.finish();

                openCL.queue.enqueueReadBuffer(cl_systemBuff, CL_TRUE, 0, sizeof(cl_SolarSystem), &solSystem);
                openCL.queue.finish();

                pDistList.resize(solSystem.planets);
                planets.resize(solSystem.planets);

                openCL.queue.enqueueReadBuffer(cl_planetBuff, CL_TRUE, 0, sizeof(cl_Planet)*solSystem.planets, planets.data());
                openCL.queue.finish();

                for (uint32_t p=0; p < solSystem.planets; ++p) {
                    const cl_Planet& planet(planets[p]);
                    moons[p].resize(planet.moons);
                    if (planet.moons > 0) {
                        //TODO: create seperate moons vector for each planet so we only read cl_moonBuff once in GenerateSolSystem()
                        openCL.queue.enqueueReadBuffer(cl_moonBuff, CL_TRUE, sizeof(cl_Moon)*MAX_MOONS*p, sizeof(cl_Moon)*planet.moons, moons[p].data());
                        openCL.queue.finish();
                    }
                }
            } else {
                pDistList.resize(0);
            }

            std::cout << "Planets:" << solSystem.planets << std::endl;
            std::cout << "Moons:" << solSystem.moons << std::endl;
            std::cout << "Civs:" << solSystem.civ_count << std::endl;
            std::cout << "Conflict:" << solSystem.conflict << std::endl;
            std::cout << "Security:" << solSystem.security << std::endl;
            std::cout << "Economy:" << solSystem.economy << std::endl;

            isStarLoaded = true;

            sound.ForcePlay(Sounds::ENTERING_SYSTEM);
        }

        lsDistFrac = camera.position.VectDist(loadedStarPos) / SECTOR_HALF;

    } else if (isStarLoaded) {

        hideNearStarSprite = false;
        lsDistFrac = camera.position.VectDist(loadedStarPos) / SECTOR_HALF;

        if (lsDistFrac < STAR_VIS_DEPTH) {
            //star is out of view but still close
        } else {
            isStarLoaded = false;
            sound.ForcePlay(Sounds::EXITING_SYSTEM);
        }
    }
}

void Game::RenderSolSystem()
{
    uint4 screenBounds;
    DVec3 lsPosRelCam;
    DVec3 posRelCam;
    uint32_t mSkip;
    static cl_uint mStart;
    static cl_uint pIndex;
    static cl_uint mIndex;
    static cl_uint spIndex;
    static cl_uint smIndex;
    static std::vector<int32_t> skipList(MAX_PLANETS);

    if (isStarLoaded) {

        lsPosRelCam = camera.PointRelCam(loadedStarPos);
        loadedDist = camera.position.VectDist(loadedStarPos);

        if (lsPosRelCam.z > 0.0) {
            lsScreenPos.x = rInfo.half_X + (lsPosRelCam.x / lsPosRelCam.z) * rInfo.cam_info.cam_foc;
            lsScreenPos.y = rInfo.half_Y + (lsPosRelCam.y / lsPosRelCam.z) * rInfo.cam_info.cam_foc;
            lsScreenPos.x = ((lsScreenPos.x / gfx.windowWidth) - 0.5f) * resolution.AspectRatio;
            lsScreenPos.y = (lsScreenPos.y / gfx.windowHeight) - 0.5f;
            cdFrac = 0.5f + (0.5f * std::pow(lsDistFrac, 2.0f));
            doLensFlare = true;
        }

        openCL.RFB_Kernel.setArg(0, cl_fragBuff);
        openCL.RFB_Kernel.setArg(1, rInfo);
        openCL.FillFragBuffRT(gfx.windowWidth, gfx.windowHeight);
        openCL.queue.finish();

        for (pIndex=0; pIndex < solSystem.planets; ++pIndex)
        {
            pDistList[pIndex] = std::make_pair(camera.position.VectDist(planets[pIndex].position), pIndex);
        }

        std::sort(pDistList.begin(), pDistList.end(),
                  [](std::pair<double,uint8_t> a, std::pair<double,uint8_t> b){ return a.first > b.first; });

        for (pIndex=0; pIndex < solSystem.planets; ++pIndex)
        {
            if (pDistList[pIndex].first > loadedDist) {
                spIndex = pDistList[pIndex].second;
                const cl_Planet& planet(planets[spIndex]);
                posRelCam = camera.PointRelCam(planet.position);
                mStart = spIndex * MAX_MOONS;
                skipList[pIndex] = pIndex;

                if (gfx.CalcBounds(camera.foclen, planet.radius*PLANET_MAG, posRelCam, screenBounds)) {
                    openCL.DP_Kernel.setArg(0, cl_planetBuff);
                    openCL.DP_Kernel.setArg(1, cl_starBuff);
                    openCL.DP_Kernel.setArg(2, cl_fragBuff);
                    openCL.DP_Kernel.setArg(3, spIndex);
                    openCL.DP_Kernel.setArg(4, loadedIndex);
                    openCL.DP_Kernel.setArg(5, rInfo);
                    openCL.DrawPlanet(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
                    openCL.queue.finish();
                }

                for (uint32_t m=0; m < planet.moons; ++m)
                {
                    mIndex = mStart + m;
                    const cl_Moon& moon(moons[spIndex][m]);
                    posRelCam = camera.PointRelCam(moon.position);

                    if (gfx.CalcBounds(camera.foclen, moon.radius*PLANET_MAG, posRelCam, screenBounds)) {
                        openCL.DM_Kernel.setArg(0, cl_moonBuff);
                        openCL.DM_Kernel.setArg(1, cl_starBuff);
                        openCL.DM_Kernel.setArg(2, cl_fragBuff);
                        openCL.DM_Kernel.setArg(3, mIndex);
                        openCL.DM_Kernel.setArg(4, loadedIndex);
                        openCL.DM_Kernel.setArg(5, rInfo);
                        openCL.DrawMoon(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
                        openCL.queue.finish();
                    }
                }
            } else {
                skipList[pIndex] = -1;
            }
        }

        // render real star sphere (disabled for now since hard to blend with flare)
        /*if (gfx.CalcBounds(camera.foclen, loadedStar.radius*STAR_MAG, lsPosRelCam, screenBounds)) {
            //TODO:create game timer (stops when game paused)
            static cl_double gTime;
            gTime = glfwGetTime() * 0.00001;
            openCL.DS_Kernel.setArg(0, cl_starBuff);
            openCL.DS_Kernel.setArg(1, cl_fragBuff);
            openCL.DS_Kernel.setArg(2, loadedIndex);
            openCL.DS_Kernel.setArg(3, gTime);
            openCL.DS_Kernel.setArg(4, rInfo);
            openCL.DrawStar(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
            openCL.queue.finish();
        }*/

        for (pIndex=0; pIndex < solSystem.planets; ++pIndex)
        {
            if ((int32_t)pIndex == skipList[pIndex]) continue;
            spIndex = pDistList[pIndex].second;
            const cl_Planet& planet(planets[spIndex]);
            mStart = spIndex * MAX_MOONS;
            mSkip = 0;

            mDistList.resize(planet.moons);
            for (uint32_t m=0; m < planet.moons; ++m)
            {
                mDistList[m] = std::make_pair(camera.position.VectDist(moons[spIndex][m].position), m);
            }

            std::sort(mDistList.begin(), mDistList.end(),
                      [](std::pair<double,uint8_t> a, std::pair<double,uint8_t> b){ return a.first > b.first; });

            for (uint32_t m=0; m < planet.moons; ++m)
            {
                if (mDistList[m].first > pDistList[pIndex].first) {

                    smIndex = mDistList[m].second;
                    mIndex = mStart + smIndex;
                    const cl_Moon& moon(moons[spIndex][smIndex]);
                    posRelCam = camera.PointRelCam(moon.position);

                    if (gfx.CalcBounds(camera.foclen, moon.radius*PLANET_MAG, posRelCam, screenBounds)) {
                        openCL.DM_Kernel.setArg(0, cl_moonBuff);
                        openCL.DM_Kernel.setArg(1, cl_starBuff);
                        openCL.DM_Kernel.setArg(2, cl_fragBuff);
                        openCL.DM_Kernel.setArg(3, mIndex);
                        openCL.DM_Kernel.setArg(4, loadedIndex);
                        openCL.DM_Kernel.setArg(5, rInfo);
                        openCL.DrawMoon(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
                        openCL.queue.finish();
                    }

                    ++mSkip;
                    continue;
                }
                break;
            }

            posRelCam = camera.PointRelCam(planet.position);

            if (gfx.CalcBounds(camera.foclen, planet.radius*PLANET_MAG, posRelCam, screenBounds)) {
                openCL.DP_Kernel.setArg(0, cl_planetBuff);
                openCL.DP_Kernel.setArg(1, cl_starBuff);
                openCL.DP_Kernel.setArg(2, cl_fragBuff);
                openCL.DP_Kernel.setArg(3, spIndex);
                openCL.DP_Kernel.setArg(4, loadedIndex);
                openCL.DP_Kernel.setArg(5, rInfo);
                openCL.DrawPlanet(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
                openCL.queue.finish();
            }

            for (uint32_t m=mSkip; m < planet.moons; ++m)
            {
                smIndex = mDistList[m].second;
                mIndex = mStart + smIndex;
                const cl_Moon& moon(moons[spIndex][smIndex]);
                posRelCam = camera.PointRelCam(moon.position);

                if (gfx.CalcBounds(camera.foclen, moon.radius*PLANET_MAG, posRelCam, screenBounds)) {
                    openCL.DM_Kernel.setArg(0, cl_moonBuff);
                    openCL.DM_Kernel.setArg(1, cl_starBuff);
                    openCL.DM_Kernel.setArg(2, cl_fragBuff);
                    openCL.DM_Kernel.setArg(3, mIndex);
                    openCL.DM_Kernel.setArg(4, loadedIndex);
                    openCL.DM_Kernel.setArg(5, rInfo);
                    openCL.DrawMoon(screenBounds.x, screenBounds.y, screenBounds.z, screenBounds.w);
                    openCL.queue.finish();
                }
            }
        }

    } else if (doLensFlare) {
        cl_InstanceData& instDat(instList.back());
        lsScreenPos = { instDat.offset.s[0], instDat.offset.s[1] };
        lsScreenPos.x = ((lsScreenPos.x / gfx.windowWidth) - 0.5f) * resolution.AspectRatio;
        lsScreenPos.y = (lsScreenPos.y / gfx.windowHeight) - 0.5f;
        loadedStarClr = { instDat.color.s[0], instDat.color.s[1], instDat.color.s[2] };
        cdFrac = 0.5f + (0.5f * std::pow(instDat.depth, 2.0f));
        ctFrac = 0.5f - std::min((loadedStar.temp / (float)STAR_MAX_TEMP) * 0.5f, 0.5f);
        lsLumiFrac = std::max(0.5f, 3.0f - std::min(3.0f, loadedStar.luminosity / (float)SOLAR_LUMI));
        lsDistFrac = instDat.depth;
        doLensFlare = true;
    }
}

void Game::RenderNearStars()
{
	// Activate star glow texture
	gfx.openGL.InitStarRender();
    gfx.openGL.BindTexture(STAR_GLOW_TEXTURE);

    // render farthest stars first
    for (int i=9; i>0; --i)
    {
        // draw star sprites to default frame buffer
        gfx.DrawInstances(idatSizes[i], 6, IBUFFER_OFFSET*i);
    }

    // draw closest star sprites (except loaded star)
    gfx.DrawInstances(idatSizes[0]-hideNearStarSprite, 6, 0);

    //TODO: render the closest star in 3D if necessary
}

void Game::Render2DGUI()
{
    static std::string pickedHexID;
    static float rad, rot, wave, timeAdd;
    static cl_float4 pickedColor;
    static size_t lockIntProg;
    static size_t scanIntProg;
    static float slideFrac;
    static float slidePix;
    static bool isPickedPlanet;

    static ftgl::FreetypeGlText pickedNameTxt;
    static ftgl::FreetypeGlText pickedTypeTxt;
    static ftgl::FreetypeGlText pickedMassTxt;
    static ftgl::FreetypeGlText pickedLumiTxt;
    static ftgl::FreetypeGlText pickedRadiusTxt;
    static ftgl::FreetypeGlText pickedVolumeTxt;
    static ftgl::FreetypeGlText pickedAreaTxt;
    static ftgl::FreetypeGlText pickedDensityTxt;
    static ftgl::FreetypeGlText pickedTempTxt;
    static ftgl::FreetypeGlText pickedDistTxt;
    static ftgl::FreetypeGlText pickedAgeTxt;

    static ftgl::FreetypeGlText pickedMassTxtVal;
    static ftgl::FreetypeGlText pickedTypeTxtVal;
    static ftgl::FreetypeGlText pickedLumiTxtVal;
    static ftgl::FreetypeGlText pickedRadiusTxtVal;
    static ftgl::FreetypeGlText pickedVolumeTxtVal;
    static ftgl::FreetypeGlText pickedAreaTxtVal;
    static ftgl::FreetypeGlText pickedDensityTxtVal;
    static ftgl::FreetypeGlText pickedTempTxtVal;
    static ftgl::FreetypeGlText pickedDistTxtVal;
    static ftgl::FreetypeGlText pickedAgeTxtVal;

    static ftgl::FreetypeGlText helpLockTxt;
    static ftgl::FreetypeGlText locationTxt;

    gfx.openGL.InitSpriteRender();

    if (showScanStats) {

        if (!planetPicked) {
            showScanStats = false;
            scanLocked = false;
        }

        if (scanLocked) {

            scanLocked = false;
            uint8_t lifeType;
            std::string strTitles[10];
            std::string strStats[10];

            if (isPickedPlanet) {
                lifeType = LifeLvlToEnum(pickedPlanet.life_level);
                strTitles[0] = LANG(STR_MOONS);
                strStats[0] = VarToStr((int)pickedPlanet.moons);
                strTitles[1] = LANG(STR_LIFE_LEVEL);
                strStats[1] = LifeTypeStr(lifeType);
                strTitles[2] = LANG(STR_SPECIES);
                strStats[2] = VarToStr((int)pickedPlanet.species)+((pickedPlanet.species==0)? "" : " "+LANG(STR_MILLION));
                strTitles[3] = LANG(STR_HOSTILITY);
                strStats[3] = FormatFltStr(VarToStr(std::min(pickedPlanet.hostility/255.0f, 1.0f)))+"%";
                strTitles[4] = LANG(STR_HABITABILITY);
                strStats[4] = FormatFltStr(VarToStr(std::min(pickedPlanet.habitability/255.0f, 1.0f)))+"%";
                strTitles[5] = LANG(STR_WATER_FRAC);
                strStats[5] = FormatFltStr(VarToStr(pickedPlanet.water_frac))+"%";
                strTitles[6] = LANG(STR_ECCENTRICITY);
                strStats[6] = FormatFltStr(VarToStr(pickedPlanet.eccentricity));
                strTitles[7] = LANG(STR_ORBIT_RAD);
                strStats[7] = FormatFltStr(VarToStr(pickedPlanet.orbit_rad))+" "+LANG(STR_M);
                strTitles[8] = LANG(STR_ORBIT_TIME);
                strStats[8] = FormatFltStr(VarToStr(pickedPlanet.orbit_time/60.0f/60.0f/24.0f))+" "+LANG(STR_DAYS);
                strTitles[9] = LANG(STR_ROT_TIME);
                strStats[9] = FormatFltStr(VarToStr(pickedPlanet.rot_time/60.0f/60.0f))+" "+LANG(STR_HOURS);
            } else {
                lifeType = LifeLvlToEnum(pickedMoon.life_level);
                strTitles[0] = LANG(STR_FUNCTION);
                strStats[0] = MoonTypeStr(pickedMoon.function);
                strTitles[1] = LANG(STR_LIFE_LEVEL);
                strStats[1] = LifeTypeStr(lifeType);
                strTitles[2] = LANG(STR_SPECIES);
                strStats[2] = VarToStr((int)pickedMoon.species)+((pickedMoon.species==0)? "" : " "+LANG(STR_MILLION));
                strTitles[3] = LANG(STR_HOSTILITY);
                strStats[3] = FormatFltStr(VarToStr(std::min(pickedMoon.hostility/255.0f, 1.0f)))+"%";
                strTitles[4] = LANG(STR_HABITABILITY);
                strStats[4] = FormatFltStr(VarToStr(std::min(pickedMoon.habitability/255.0f, 1.0f)))+"%";
                strTitles[5] = LANG(STR_WATER_FRAC);
                strStats[5] = FormatFltStr(VarToStr(pickedMoon.water_frac))+"%";
                strTitles[6] = LANG(STR_ECCENTRICITY);
                strStats[6] = FormatFltStr(VarToStr(pickedMoon.eccentricity));
                strTitles[7] = LANG(STR_ORBIT_RAD);
                strStats[7] = FormatFltStr(VarToStr(pickedMoon.orbit_rad))+" "+LANG(STR_M);
                strTitles[8] = LANG(STR_ORBIT_TIME);
                strStats[8] = FormatFltStr(VarToStr(pickedMoon.orbit_time/60.0f/60.0f/24.0f))+" "+LANG(STR_DAYS);
                strTitles[9] = LANG(STR_ROT_TIME);
                strStats[9] = FormatFltStr(VarToStr(pickedMoon.rot_time/60.0f/60.0f))+" "+LANG(STR_HOURS);
            }

            switch (lifeType) {
                case SINGLE_CELLED: sound.ForcePlay(Sounds::SINGLE_CELL_DETECTED); break;
                case SIMPLE_LIFE: sound.ForcePlay(Sounds::SIMPLE_MCELL_DETECTED); break;
                case COMPLEX_LIFE: sound.ForcePlay(Sounds::COMPLEX_MCELL_DETECTED); break;
                case PRIMITIVE_CIV: sound.ForcePlay(Sounds::P_CIV_DETECTED); break;
                case TYPE_0_CIV: sound.ForcePlay(Sounds::T0_CIV_DETECTED); break;
                case TYPE_I_CIV: sound.ForcePlay(Sounds::T1_CIV_DETECTED); break;
                case TYPE_II_CIV: sound.ForcePlay(Sounds::T2_CIV_DETECTED); break;
                case TYPE_III_CIV: sound.ForcePlay(Sounds::T3_CIV_DETECTED); break;
            }

            pickedTypeTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[0]+":");
            pickedTypeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[0]);

            pickedMassTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[1]+":");
            pickedMassTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[1]);

            pickedLumiTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[2]+":");
            pickedLumiTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[2]);

            pickedRadiusTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[3]+":");
            pickedRadiusTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[3]);

            pickedVolumeTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[4]+":");
            pickedVolumeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[4]);

            pickedAreaTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[5]+":");
            pickedAreaTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[5]);

            pickedDensityTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[6]+":");
            pickedDensityTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[6]);

            pickedTempTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[7]+":");
            pickedTempTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[7]);

            pickedDistTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[8]+":");
            pickedDistTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[8]);

            pickedAgeTxt.setText(&fontStyles[FONT_XOLONIUM].description, strTitles[9]+":");
            pickedAgeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, strStats[9]);
        }

    } else if (doPickStar) {

        doPickStar = false;
        pickedStarIndex = -1;
        uint32_t tmpUint = UINT_MAX;
        cl_float2 tempVec = { mouse.GetMouseX(), rInfo.pixels_Y - mouse.GetMouseY() };

        openCL.queue.enqueueWriteBuffer(cl_volatileBuff, CL_TRUE, 0, sizeof(cl_uint), &tmpUint);
        openCL.queue.enqueueWriteBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(cl_int), &pickedStarIndex);
        openCL.queue.enqueueWriteBuffer(cl_mousePosBuff, CL_TRUE, 0, sizeof(cl_float2), &tempVec);
        openCL.queue.finish();

        openCL.PS_Kernel.setArg(0, cl_starBuff);
        openCL.PS_Kernel.setArg(1, cl_volatileBuff);
        openCL.PS_Kernel.setArg(2, cl_indexBuff);
        openCL.PS_Kernel.setArg(3, cl_mousePosBuff);
        openCL.PS_Kernel.setArg(4, rInfo);

        openCL.PickStar();
        openCL.queue.finish();

        openCL.queue.enqueueReadBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(cl_int), &pickedStarIndex);
        openCL.queue.finish();

        if (pickedStarIndex > -1) {
            if (!starPicked) {
                starPicked = true;
                timeAdd = 0.0;
            }

            openCL.queue.enqueueReadBuffer(cl_starBuff, CL_TRUE, pickedStarIndex*sizeof(cl_Star), sizeof(cl_Star), &pickedStar);
            openCL.queue.finish();
            pickedColor = { pickedStar.color.red/255.0f, pickedStar.color.green/255.0f, pickedStar.color.blue/255.0f, 1.0f };

            pickedGlobPos = Index1Dto3D(pickedStarIndex, SECTOR_SPAN);
            pickedGlobPos.x += globPos.x;
            pickedGlobPos.y += globPos.y;
            pickedGlobPos.z += globPos.z;

            pickedGlobID = Index3Dto1D_64(pickedGlobPos, GALAXY_SPAN);
            pickedHexID = IntToHex(pickedGlobID);
            pickedNameTxt.setText(&fontStyles[FONT_XOLONIUM].description, std::string(16-pickedHexID.length(), ' ')+LANG(STR_STAR)+" #"+pickedHexID);

            pickedTypeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_TYPE)+":");
            pickedTypeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, StarTypeStr(pickedStar.type));

            pickedMassTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_MASS)+":");
            pickedMassTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.mass))+" "+LANG(STR_KG));

            pickedLumiTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_LUMI)+":");
            pickedLumiTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.luminosity))+" "+LANG(STR_W));

            pickedRadiusTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_RADIUS)+":");
            pickedRadiusTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.radius))+" "+LANG(STR_M));

            pickedVolumeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_VOLUME)+":");
            pickedVolumeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.volume))+" "+LANG(STR_M)+"\u00B3");

            pickedAreaTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_AREA)+":");
            pickedAreaTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.area))+" "+LANG(STR_M)+"\u00B2");

            pickedDensityTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_DENSITY)+":");
            pickedDensityTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.density))+" "+LANG(STR_KG)+"/"+LANG(STR_M)+"\u00B3");

            pickedTempTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_TEMP)+":");
            pickedTempTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(pickedStar.temp))+" "+LANG(STR_K));

            pickedDistTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_DISTANCE)+":");

            pickedAgeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_AGE)+":");
            pickedAgeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description,
                                    ((pickedStar.age<1000) ? FormatFltStr(VarToStr(pickedStar.age))+" "+LANG(STR_MILLION) :
                                     FormatFltStr(VarToStr(pickedStar.age*0.001f))+" "+LANG(STR_BILLION))+" "+LANG(STR_YEARS));
        } else {
            starPicked = false;
        }
    } else if (doPickPlanet) {

        doPickPlanet = false;
        int2 pickedIndexPM = {-1,-1};
        uint32_t tmpUint = UINT_MAX;
        cl_float2 tempVec = { mouse.GetMouseX(), rInfo.pixels_Y - mouse.GetMouseY() };

        openCL.queue.enqueueWriteBuffer(cl_volatileBuff, CL_TRUE, 0, sizeof(cl_uint), &tmpUint);
        openCL.queue.enqueueWriteBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(int2), &pickedIndexPM);
        openCL.queue.enqueueWriteBuffer(cl_mousePosBuff, CL_TRUE, 0, sizeof(cl_float2), &tempVec);
        openCL.queue.finish();

        openCL.PP_Kernel.setArg(0, cl_planetBuff);
        openCL.PP_Kernel.setArg(1, cl_moonBuff);
        openCL.PP_Kernel.setArg(2, cl_volatileBuff);
        openCL.PP_Kernel.setArg(3, cl_indexBuff);
        openCL.PP_Kernel.setArg(4, cl_mousePosBuff);
        openCL.PP_Kernel.setArg(5, rInfo);

        openCL.PickPlanet(solSystem.planets);
        openCL.queue.finish();

        openCL.queue.enqueueReadBuffer(cl_indexBuff, CL_TRUE, 0, sizeof(int2), &pickedIndexPM);
        openCL.queue.finish();

        isPickedPlanet = pickedIndexPM.x > -1;

        if (isPickedPlanet || pickedIndexPM.y > -1) {

            std::string titleStr, typeStr, massStr, velStr, radStr, volStr, areaStr, densStr, tempStr, ageStr;

            if (!planetPicked) {
                planetPicked = true;
                timeAdd = 0.0;
            }

            if (isPickedPlanet) {
                pickedPlanetIndex = pickedIndexPM.x;
                openCL.queue.enqueueReadBuffer(cl_planetBuff, CL_TRUE, pickedPlanetIndex*sizeof(cl_Planet), sizeof(cl_Planet), &pickedPlanet);
                openCL.queue.finish();

                titleStr = "             "+LANG(STR_PLANET)+" #"+VarToStr(pickedPlanetIndex);
                typeStr = PlanetTypeStr(pickedPlanet.type);
                massStr = FormatFltStr(VarToStr(pickedPlanet.mass))+" "+LANG(STR_KG);
                velStr = FormatFltStr(VarToStr(pickedPlanet.velocity))+" "+LANG(STR_M)+"/"+LANG(STR_S);
                radStr = FormatFltStr(VarToStr(pickedPlanet.radius))+" "+LANG(STR_M);
                volStr = FormatFltStr(VarToStr(pickedPlanet.volume))+" "+LANG(STR_M)+"\u00B3";
                areaStr = FormatFltStr(VarToStr(pickedPlanet.area))+" "+LANG(STR_M)+"\u00B2";
                densStr = FormatFltStr(VarToStr(pickedPlanet.density))+" "+LANG(STR_KG)+"/"+LANG(STR_M)+"\u00B3";
                tempStr = FormatFltStr(VarToStr(pickedPlanet.temp))+" "+LANG(STR_K);
                ageStr = ((pickedPlanet.age<1000) ? FormatFltStr(VarToStr(pickedPlanet.age))+" "+LANG(STR_MILLION) :
                          FormatFltStr(VarToStr(pickedPlanet.age*0.001f))+" "+LANG(STR_BILLION))+" "+LANG(STR_YEARS);
            } else {
                pickedPlanetIndex = pickedIndexPM.y;
                openCL.queue.enqueueReadBuffer(cl_moonBuff, CL_TRUE, pickedPlanetIndex*sizeof(cl_Moon), sizeof(cl_Moon), &pickedMoon);
                openCL.queue.finish();

                titleStr = "     "+LANG(STR_MOON)+" #"+VarToStr(pickedPlanetIndex%MAX_MOONS)+" ["+LANG(STR_PLANET)+" #"+VarToStr(pickedPlanetIndex/MAX_MOONS)+"]";
                typeStr = MoonTypeStr(pickedMoon.type);
                massStr = FormatFltStr(VarToStr(pickedMoon.mass))+" "+LANG(STR_KG);
                velStr = FormatFltStr(VarToStr(pickedMoon.velocity))+" "+LANG(STR_M)+"/"+LANG(STR_S);
                radStr = FormatFltStr(VarToStr(pickedMoon.radius))+" "+LANG(STR_M);
                volStr = FormatFltStr(VarToStr(pickedMoon.volume))+" "+LANG(STR_M)+"\u00B3";
                areaStr = FormatFltStr(VarToStr(pickedMoon.area))+" "+LANG(STR_M)+"\u00B2";
                densStr = FormatFltStr(VarToStr(pickedMoon.density))+" "+LANG(STR_KG)+"/"+LANG(STR_M)+"\u00B3";
                tempStr = FormatFltStr(VarToStr(pickedMoon.temp))+" "+LANG(STR_K);
                ageStr = ((pickedMoon.age<1000) ? FormatFltStr(VarToStr(pickedMoon.age))+" "+LANG(STR_MILLION) :
                          FormatFltStr(VarToStr(pickedMoon.age*0.001f))+" "+LANG(STR_BILLION))+" "+LANG(STR_YEARS);
            }
            pickedColor = { 0.95f, 0.95f, 1.0f, 1.0f };

            pickedNameTxt.setText(&fontStyles[FONT_XOLONIUM].description, titleStr);

            pickedTypeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_TYPE)+":");
            pickedTypeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, typeStr);

            pickedMassTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_MASS)+":");
            pickedMassTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, massStr);

            pickedLumiTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_VELOCITY)+":");
            pickedLumiTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, velStr);

            pickedRadiusTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_RADIUS)+":");
            pickedRadiusTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, radStr);

            pickedVolumeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_VOLUME)+":");
            pickedVolumeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, volStr);

            pickedAreaTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_AREA)+":");
            pickedAreaTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, areaStr);

            pickedDensityTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_DENSITY)+":");
            pickedDensityTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, densStr);

            pickedTempTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_TEMP)+":");
            pickedTempTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, tempStr);

            pickedAgeTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_AGE)+":");
            pickedAgeTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, ageStr);

            pickedDistTxt.setText(&fontStyles[FONT_XOLONIUM].description, LANG(STR_DISTANCE)+":");

        } else {
            planetPicked = false;
        }
    }

    if (starPicked || planetPicked) {

        if (starPicked) {
            pickedPos = camera.PointRelCam(pickedStar.position);
            pickedDist = pickedPos.VectMag();
            rad = std::max(1.0, log(1.0 + (pickedStar.luminosity / (ISL_DIST_MULT * std::pow(pickedDist, 2.0)))));
            if (isStarLoaded && loadedGlobID == pickedGlobID) {
                pickedDistTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(float(pickedDist/ONE_AU)))+" "+LANG(STR_AU));
            } else {
                pickedDistTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(float(pickedDist/LIGHT_YEAR)))+" "+LANG(STR_LY));
            }
        } else {
            if (isPickedPlanet) {
                pickedPos = camera.PointRelCam(pickedPlanet.position);
                pickedDist = pickedPos.VectMag();
                rad = 0.0f;
            } else {
                pickedPos = camera.PointRelCam(pickedMoon.position);
                pickedDist = pickedPos.VectMag();
                rad = 0.0f;
            }
            if (!showScanStats && !scanLocked) {
                pickedDistTxtVal.setText(&fontStyles[FONT_XOLONIUM].description, FormatFltStr(VarToStr(float(pickedDist/ONE_AU)))+" "+LANG(STR_AU));
            }
        }

        timeAdd += deltaTime * 0.001;
        slideFrac = std::min(1.0f, std::max(-0.3f, -0.3f + timeAdd * GUISLIDE_SPEED));
        slidePix = 226.f - (slideFrac * 226.f);

        if (pickedPos.z > 0.0f && rad > 0.0f && !isStarLoaded) {
            lsScreenXY.s[0] = rInfo.half_X + (pickedPos.x / pickedPos.z) * rInfo.cam_info.cam_foc;
            lsScreenXY.s[1] = rInfo.half_Y + (pickedPos.y / pickedPos.z) * rInfo.cam_info.cam_foc;
            wave = std::fabs(std::sin(timeAdd));
            rot = camLocked ? LimitAngle(rot + float(deltaTime * 0.001)) : 0.0f;
            gfx.openGL.BindTexture(PICK_RING_TEXTURE);
            pickedColor.s[3] = (wave*0.5f) + 0.2f;
            gfx.SpriteConfig(pickedColor, lsScreenXY, rad*STAR_TEX_RATIO, rot, 0.3);
            gfx.DrawSprite();
        }

        if (camLocked) {
            if (planetPicked) {
                if (scanLocked) {
                    scanProgress = scanTimer.MilliCount() / SCAN_TIME_MS;
                    scanIntProg = std::round(12.0f * scanProgress);
                    if (scanProgress < 1.0f) {
                        std::string spaces(13-scanIntProg, ' ');
                        std::string spad(scanIntProg, ' ');
                        helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, spad+"\u2772 "+spaces+LANG(STR_SCANNING)+spaces+" \u2773");
                    } else {
                        showScanStats = true;
                    }
                } else if (!showScanStats) {
                    helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, "    "+LANG(STR_HELP_ANAL));
                } else {
                    helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, "");
                }
            } else {
                helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, "             \u2772 "+LANG(STR_LOCKED)+" \u2773");
            }
        } else if (camLock) {
            lockIntProg = std::round(6.0f * lockProgress);
            std::string spaces(lockIntProg, ' ');
            std::string spad(13-lockIntProg, ' ');
            helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, spad+"\u2772 "+spaces+LANG(STR_LOCKING)+spaces+" \u2773");
        } else {
            helpLockTxt.setText(&fontStyles[FONT_XOLONIUM].description, FormatStr(LANG(STR_HELP_LOCK), GLFW::GetInputStr(KEY_MAP(KEY_TARG_LOCK))));
        }

        gfx.openGL.BindTexture(INFO_PANEL_TEXTURE);
        gfx.SpriteConfig(CL_WHITE, {gfx.windowWidth-(slideFrac*226.f), gfx.windowHeight-226.f}, 226.0f, 0.0f, 0.2);
        gfx.DrawSprite();
    }

    gfx.openGL.InitTextRender();

    if (starPicked || planetPicked) {
        pickedNameTxt.setScaleAndPos(gfx.fontSizeMedium, gfx.windowWidth+slidePix-285.f, gfx.windowHeight-6.f, 0.1f);
        gfx.DrawText(pickedNameTxt);

        pickedTypeTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-70.f, 0.1f);
        pickedTypeTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-70.f, 0.1f);
        gfx.DrawText(pickedTypeTxt);
        gfx.DrawText(pickedTypeTxtVal);

        pickedMassTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-100.f, 0.1f);
        pickedMassTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-100.f, 0.1f);
        gfx.DrawText(pickedMassTxt);
        gfx.DrawText(pickedMassTxtVal);

        pickedLumiTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-130.f, 0.1f);
        pickedLumiTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-130.f, 0.1f);
        gfx.DrawText(pickedLumiTxt);
        gfx.DrawText(pickedLumiTxtVal);

        pickedRadiusTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-160.f, 0.1f);
        pickedRadiusTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-160.f, 0.1f);
        gfx.DrawText(pickedRadiusTxt);
        gfx.DrawText(pickedRadiusTxtVal);

        pickedVolumeTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-190.f, 0.1f);
        pickedVolumeTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-190.f, 0.1f);
        gfx.DrawText(pickedVolumeTxt);
        gfx.DrawText(pickedVolumeTxtVal);

        pickedAreaTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-220.f, 0.1f);
        pickedAreaTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-220.f, 0.1f);
        gfx.DrawText(pickedAreaTxt);
        gfx.DrawText(pickedAreaTxtVal);

        pickedDensityTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-250.f, 0.1f);
        pickedDensityTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-250.f, 0.1f);
        gfx.DrawText(pickedDensityTxt);
        gfx.DrawText(pickedDensityTxtVal);

        pickedTempTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-280.f, 0.1f);
        pickedTempTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-280.f, 0.1f);
        gfx.DrawText(pickedTempTxt);
        gfx.DrawText(pickedTempTxtVal);

        pickedDistTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-310.f, 0.1f);
        pickedDistTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-310.f, 0.1f);
        gfx.DrawText(pickedDistTxt);
        gfx.DrawText(pickedDistTxtVal);

        pickedAgeTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-280.f, gfx.windowHeight-340.f, 0.1f);
        pickedAgeTxtVal.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-170.f, gfx.windowHeight-340.f, 0.1f);
        gfx.DrawText(pickedAgeTxt);
        gfx.DrawText(pickedAgeTxtVal);

        helpLockTxt.setScaleAndPos(gfx.fontSizeSmall, gfx.windowWidth+slidePix-260.f, gfx.windowHeight-385.f, 0.1f);
        gfx.DrawText(helpLockTxt);
    }

    if (isStarLoaded) {
        locationTxt.setText(&fontStyles[FONT_OXANIUM].description, LANG(STR_SOLAR_SYS));
    } else {
        locationTxt.setText(&fontStyles[FONT_OXANIUM].description, LANG(STR_INT_SPACE));
    }

    locationTxt.setScaleAndPos(gfx.fontSizeMedium, 20, gfx.windowHeight-20, 0.1f);
    gfx.DrawText(locationTxt);

    gfx.openGL.StopTextRenderer();
}

void Game::RenderSprites()
{
    // render closer stars with OGL
    RenderNearStars();

    // draw ray-traced frame as textured quad
    if (isStarLoaded) {
        gfx.openGL.InitFrameRender();
        gfx.DrawFrameRT();
    }

    // do lense flare if close enough to star
    if (doLensFlare) {
        gfx.openGL.InitFlareRender(loadedStarClr, lsScreenPos, lsLumiFrac, resolution.AspectRatio, glfwGetTime(), cdFrac, ctFrac, (FLARE_VIS_DEPTH-lsDistFrac)*FLARE_ADM);
        gfx.DrawFrameTex();
    }
}

void Game::FinishFrameCL()
{
	// compute final pixel colors from frags
	openCL.FF_Kernel.setArg(0, cl_fragBuff);
	openCL.FF_Kernel.setArg(1, gfx.ClFrameMemory(0));
	openCL.FF_Kernel.setArg(2, rInfo);
	openCL.FragsToFrame(gfx.windowWidth, gfx.windowHeight);
	openCL.queue.finish();

    //gfx.ReleaseGLBuffers(openCL);
    gfx.BlitFrame();
}

void Game::FinishFrameRT()
{
	openCL.RFF_Kernel.setArg(0, cl_fragBuff);
	openCL.RFF_Kernel.setArg(1, gfx.ClFrameMemory(1));
	openCL.RFF_Kernel.setArg(2, rInfo);
	openCL.FragsToFrameRT(gfx.windowWidth, gfx.windowHeight);
	openCL.queue.finish();
}

void Game::ComposeFrame()
{
    //TODO: gen planet textures/bump map (gl tex)
    //TODO: render planet orbit paths (https://github.com/memononen/nanovg)
    //TODO: adjust planet orbits to prevent overlapping
    //TODO: enable planet orbits & rotation
    //TODO: show elements on planets/moon
    //TODO: render dust/gas clouds
    //TODO: render asteroids
    //TODO: use quaternions for rotation
    //TODO: full support for unicode font glyphs

	// handle keyboard/mouse actions
	HandleInput();

	// update camera data
	UpdateCamera();

    // reset/update some stuff
    BeginActions();

    // give OCL control of OGL buffers
    gfx.AcquireGLBuffers(openCL);

    // render distant stars using OCL
    RenderFarStars();

    // compute final pixel colors for OCL frame
    FinishFrameCL();

    // generate planets, moons, etc
    GenerateSolSystem();

    // ray-trace planets, moons, sun with OCL
    RenderSolSystem();

    // finalize ray-traced frame texture
    FinishFrameRT();

    // make OCL release control of OGL memory
    gfx.ReleaseGLBuffers(openCL);

    // render near stars and other sprites
    RenderSprites();

    // render 2D GUI components
    Render2DGUI();
}
