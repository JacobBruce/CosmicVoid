__kernel void GenStars(__global Star* star_buffer, RenderInfo render_info)
{
    uint zone_X = get_global_id(0);
	uint zone_Y = get_global_id(1);
	uint zone_Z = get_global_id(2);
	uint zone_index = zone_X + SECTOR_SPAN * (zone_Y + SECTOR_SPAN * zone_Z);
	
    long glob_X = (render_info.zone_offset.x + zone_X) % GALAXY_SPAN;
	long glob_Y = (render_info.zone_offset.y + zone_Y) % GALAXY_SPAN;
	long glob_Z = (render_info.zone_offset.z + zone_Z) % GALAXY_SPAN;
	ulong glob_index = glob_X + GALAXY_SPAN * (glob_Y + GALAXY_SPAN * glob_Z);

	double variFrac, sM, sT, sR;
	float denseFrac, maxDF;
	FloatVec8 starStats;
	UCharVec8 starColDat;
	ushort2 starAgeDat;
	LongBytes seed;
	ulong4 seeds;
	Star star;
	
	star.position.x = (15.0 - (double)zone_X) * ZONE_WIDTH;
	star.position.y = (15.0 - (double)zone_Y) * ZONE_WIDTH;
	star.position.z = (15.0 - (double)zone_Z) * ZONE_WIDTH;

	seeds.x = RandomLong(glob_index);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	seed.data = seeds.x;
	star.type = starTypeProbs[seed.bytes[3]];
	starStats.data = starTypeStats[star.type];
	starColDat.data = starAvgColors[star.type];
	starAgeDat = starAgeLimits[star.type];

	variFrac = RandDblFromLng(seeds.y);
	sM = starStats.s[MASS_INDEX] + (starStats.s[MVAR_INDEX] * variFrac);
	variFrac = RandDblFromLng(seeds.z);
	sR = starStats.s[RADI_INDEX] + (starStats.s[RVAR_INDEX] * variFrac);
	
	star.age = starAgeDat.x + (seeds.w % starAgeDat.y);
	star.mass = SOLAR_MASS * sM;
	star.radius = SOLAR_RADI * sR;
	star.volume = PI_MUL_4o3 * pow(star.radius, 3.0f);
	star.density = star.mass / star.volume;
	
	variFrac = fmin(10.0, fmax(0.1, star.mass / SOLAR_MASS));
	star.rot_time = RandDblFromLng(seeds.w) * MAX_ROT_TIME * variFrac;
	//TODO: adjust rotation speed based on star type
	star.rot_time += (star.rot_time < 0.0f) ? -MIN_ROT_TIME : MIN_ROT_TIME;

	denseFrac = sM / sR;
	maxDF = starStats.s[MAXM_INDEX] / starStats.s[MINR_INDEX];
	variFrac = denseFrac / maxDF;
	sT = starStats.s[TEMP_INDEX] + (starStats.s[TVAR_INDEX] * variFrac);
	star.temp = SOLAR_TEMP * sT;
	star.luminosity = SOLAR_LUMI * pow(sR,2.0) * pow(sT,4.0);

	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);
	
	seed.data = seeds.w;
	star.color.red = starColDat.s[0] + (seed.bytes[0] % starColDat.s[3]);
	star.color.green = starColDat.s[1] + (seed.bytes[1] % starColDat.s[4]);
	star.color.blue = starColDat.s[2] + (seed.bytes[2] % starColDat.s[5]);
	star.color.alpha = 255;
	
	star.position.x += (float)(seeds.x % 16UL) * ZONE_SPLIT;
	star.position.y += (float)(seeds.y % 16UL) * ZONE_SPLIT;
	star.position.z += (float)(seeds.z % 16UL) * ZONE_SPLIT;
	
	star_buffer[zone_index] = star;
}

__kernel void GenPlanets(__global SolarSystem* solar_system,
__global Planet* planet_buffer, __global Moon* moon_buffer,
__global Star* star_buffer, __global uint* loaded_star)
{
    uint index = get_global_id(0);
	ulong seed = solar_system->index * (1+index);

	ShortBytes age;
	MoonData moonDat;
	uchar moons, function, species, hostility;
	uchar habitability, life_level, type;
	
	double pD, pR, pR2, pVol;
	float variFrac, lumiFrac, orbitGap;
	float frac1, frac2, rotNP;//,randFlt
	uint mindex, mstart;
	uint sumHostile;
	FloatVec4 bodyStats;
	uchar lifeType;
	ulong4 seeds;
	Planet planet;
	Moon moon;
	
	Star star = star_buffer[*loaded_star];
	rotNP = star.rot_time < 0.0 ? -1.0 : 1.0;

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
	pR2 = pow(planet.radius, 2.0f);
	pVol = PI_MUL_4o3 * pR2 * planet.radius;
	planet.area = PI_MUL_4 * pR2;
	planet.mass = planet.density * pVol;
	planet.gravity = G * planet.mass / pR2;
	
	lumiFrac = star.luminosity / SOLAR_LUMI;
	frac1 = fmax(planet.mass / EARTH_MASS, 0.3);
	frac2 = star.radius * 1.25f;
	//frac2 = (star.mass / SOLAR_MASS) * 1000.0f;
	orbitGap = fmax(frac2, (float)((G * star.mass) / 100000000.0) - frac2) / MAX_PLANETS;
	planet.orbit_rad = frac2 + (index * orbitGap) + (fabs(RandDblFromLng(seeds.w)) * orbitGap * 0.95f);
	//planet.orbit_rad = G * star.mass / pow(planet.velocity, 2.0f);
	
	seeds.x = RandomLong(seed);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);
	
	planet.eccentricity = 0.00001 + (fabs(RandDblFromLng(seeds.x)) * ((seeds.y%4) ? 0.01 : 0.49));
	planet.velocity = ((G * star.mass) / planet.orbit_rad) * rotNP;
	planet.temp = fmax(5.0, lumiFrac * (EARTH_TEMP / pow(planet.orbit_rad / ONE_AU, 2.0)));
    planet.orbit_time = (PI_MUL_2 * planet.orbit_rad) / fabs(planet.velocity);
	rotNP = RandDblFromLng(seeds.z) > 0.8 ? -rotNP : rotNP;
	planet.rot_time = (3600.0 + ((seeds.w % 170000) * frac1)) * rotNP;
	planet.position.xyz = star.position.xyz;
	planet.position.x += planet.orbit_rad;
	
	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);
	
	planet.orbit.x = pow(RandDblFromLng(seeds.x), 2.0) * MAX_ORBIT_TILT;
	planet.orbit.y = pow(RandDblFromLng(seeds.y), 2.0) * MAX_ORBIT_TILT;

	if (type == 3 || type == 4) { // ICE TYPE
		if (planet.temp > FREEZING_TEMP) {
			if (planet.temp < BOILING_TEMP) {
				type = 5;
			} else {
				type = (seeds.x%2) ? 6 : 7;
			}
		}
	}
	
	if (type == 5) { // WATER TYPE
		if (planet.temp < FREEZING_TEMP) {
			type = 3;
		} else if (planet.temp > BOILING_TEMP) {
			type = (seeds.x%2) ? 6 : 7;
		}
	}

	if (type == 7) { // THEORETICAL
		if (planet.temp > LAVA_MIN_TEMP) {
			type = 9;
		} else {
			type = 10 + (seeds.y%2);
		}
	}
	
	if (type == 6) { // EARTH-LIKE
		if (planet.temp < FREEZING_TEMP) {
			type = 3;
		} else if (planet.temp > BOILING_TEMP) {
			type = 7 + (seeds.y%2);
		}
	}
	
	switch (type) {
	case 0: //Dwarf planet
		variFrac = 0.1f;
		planet.water_frac = 0.0001f;
		planet.temp *= (90 + (seeds.w%20)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DUST_ATMOS];
		planet.atmos_spec.alpha += seeds.y%10;
		habitability = 40 + (seeds.z%3);
		break;
	case 1: //Gas dwarf
		variFrac = 0.3f;
		planet.water_frac = 0.001f;
		planet.temp *= (80 + (seeds.w%20)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[GAS_ATMOS];
		planet.atmos_spec.alpha += 40 + (seeds.y%10);
		habitability = 1 + (seeds.z%3);
		break;
	case 2: //Gas giant
		variFrac = 0.3f;
		planet.water_frac = 0.001f;
		planet.temp *= (80 + (seeds.w%20)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[GAS_ATMOS];
		planet.atmos_spec.alpha += 60 + (seeds.y%10);
		habitability = 1 + (seeds.z%3);
		break;
	case 3: //Ice planet
		variFrac = 0.6f;
		planet.water_frac = 0.4f;
		planet.temp *= (80 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DEF_ATMOS];
		planet.atmos_spec.alpha += seeds.y%20;
		habitability = 30 + (seeds.z%3);
		break;
	case 4: //Ice giant
		variFrac = 0.6f;
		planet.water_frac = 0.4f;
		planet.temp *= (80 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DEF_ATMOS];
		planet.atmos_spec.alpha += 10 + (seeds.y%20);
		habitability = 30 + (seeds.z%3);
		break;
	case 5: //Water planet
		variFrac = 0.1f;
		planet.water_frac = 0.9f;
		planet.temp *= (90 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DEF_ATMOS];
		planet.atmos_spec.alpha += 10 + (seeds.y%20);
		habitability = 64 + (seeds.z%3);
		break;
	case 6: //Earth-like planet
		variFrac = 0.5f;
		planet.water_frac = 0.4f;
		planet.temp *= (98 + (seeds.w%5)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[LIFE_ATMOS];
		planet.atmos_spec.alpha += 30 + (seeds.y%20);
		habitability = 125 + (seeds.z%3);
		break;
	case 7: //Desert planet
		variFrac = 0.01f;
		planet.water_frac = 0.00001f;
		planet.temp *= (100 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DUST_ATMOS];
		planet.atmos_spec.alpha += 20 + (seeds.y%20);
		habitability = 20 + (seeds.z%3);
		break;
	case 8: //Silicate planet
		variFrac = 0.2f;
		planet.water_frac = 0.0001f;
		planet.temp *= (96 + (seeds.w%9)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DEF_ATMOS];
		planet.atmos_spec.alpha += 10 + (seeds.y%30);
		habitability = 50 + (seeds.z%3);
		break;
	case 9: //Lava planet
		variFrac = 0.0001f;
		planet.water_frac = 0.000001f;
		planet.temp *= (110 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[FIRE_ATMOS];
		planet.atmos_spec.alpha += 30 + (seeds.y%30);
		habitability = 5 + (seeds.z%3);
		break;
	case 10: //Carbon planet
		variFrac = 0.001f;
		planet.water_frac = 0.00001f;
		planet.temp *= (100 + (seeds.w%10)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DUST_ATMOS];
		planet.atmos_spec.alpha += seeds.y%30;
		habitability = 15 + (seeds.z%3);
		break;
	case 11: //Iron planet
		variFrac = 0.001f;
		planet.water_frac = 0.00001f;
		planet.temp *= (90 + (seeds.w%20)) / 100.0f;
		planet.atmos_spec.rgba = atmosphereColors[DUST_ATMOS];
		planet.atmos_spec.alpha += seeds.y%20;
		habitability = 10 + (seeds.z%3);
		break;
	}
	
	seeds.x = RandomLong(seeds.w);
	seeds.y = RandomLong(seeds.x);
	seeds.z = RandomLong(seeds.y);
	seeds.w = RandomLong(seeds.z);

	frac1 = fabs(RandDblFromLng(seeds.x));
    frac2 = min(max(planet.temp-EARTH_TEMP,0.f) / (LAVA_MIN_TEMP-BOILING_TEMP), 1.f);
	planet.water_frac = (planet.water_frac + (frac1 * variFrac)) * (1.f - frac2);
	//TODO: use planet gravity/mass to influence habitability,
	// account for system star (point in lifecycle, radiation, etc)
	habitability += (64.0f-(fabs(fmin(2.0f,(planet.temp/EARTH_TEMP))-1.0f)*64.0f)) + fmin(64.0f,planet.water_frac*128.0f);
	age.data = star.age * (0.8 + fabs(RandDblFromLng(seeds.y) * 0.19));
	//TODO: calc terraformed and colonized planets/moons on CPU
	life_level = (seeds.y % habitability) * fmin(1.0f, age.data / EARTH_AGED);
	lifeType = LifeLvlToEnum(life_level);
	species = (seeds.z % life_level) * lifeType * 0.1f;
	hostility = (lifeType > COMPLEX_LIFE) ? (fabs(RandDblFromLng(seeds.z)) * 160.0) + (255-life_level) : 0;
	sumHostile = hostility;
	moons = seeds.w % MAX_MOONS;

	variFrac = planet.gravity / MAX_GRAVITY;
	planet.atmos_radi = planet.radius * (1.2 - (0.15f * variFrac));
	planet.atmos_spec.red += 10.0f * (planet.temp/LAVA_MAX_TEMP);
	planet.atmos_spec.green += (5.0f * (habitability/255.0f)) + (5.0f * (species/(float)MAX_MILL_SPECIES));
	planet.atmos_spec.blue += (5.0f * fmax(0.0f, 1.0f-(planet.temp/BOILING_TEMP))) + (5.0f * planet.water_frac);
	planet.atmos_spec.alpha += (60.0f * variFrac) + (60.0f * (life_level/255.0f)) + (10.0f * RandDblFromLng(seeds.w));

	planet.info = (uchar8)(age.bytes[0], age.bytes[1], moons, species, hostility, habitability, life_level, type);

	//frac2 = (planet.mass / EARTH_MASS) * 100.0;
	orbitGap = planet.radius * 2.0;
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
		pR2 = pow(moon.radius, 2.0f);
		pVol = PI_MUL_4o3 * pR2 * moon.radius;
		moon.area = PI_MUL_4 * pR2;
		moon.mass = moon.density * pVol;
		moon.gravity = G * moon.mass / pR2;

		orbitGap += fabs(RandDblFromLng(seeds.w)) * MAX_MOON_GAP;
		moon.eccentricity = 0.000001 + (fabs(RandDblFromLng(seeds.x)) * 0.1);
		moon.orbit_rad = moon.radius + orbitGap;
		moon.velocity = ((G * planet.mass) / moon.orbit_rad) * rotNP;
		orbitGap += moon.radius * 2.0;
		
		seeds.z = RandomLong(seeds.w);
		seeds.w = RandomLong(seeds.z);
		
		frac1 = RandDblFromLng(seeds.z) * 0.99;
		moon.temp = fmax(2.0, lumiFrac * (MOON_TEMP / pow(planet.orbit_rad / ONE_AU, 2.0)));
		moon.orbit_time = (PI_MUL_2 * moon.orbit_rad) / moon.velocity;
		frac2 = RandDblFromLng(seeds.w) > 0.9 ? -rotNP : rotNP;
		moon.rot_time = (planet.rot_time + (planet.rot_time*frac1)) * frac2;
		moon.position.xyz = planet.position.xyz;
		moon.position.z += moon.orbit_rad*PLANET_MAG*0.1;

		seeds.x = RandomLong(seeds.w);
		seeds.y = RandomLong(seeds.x);
		seeds.z = RandomLong(seeds.y);
		seeds.w = RandomLong(seeds.z);

		moon.orbit.x = pow(RandDblFromLng(seeds.x), 2.0) * MAX_ORBIT_TILT;
		moon.orbit.y = pow(RandDblFromLng(seeds.y), 2.0) * MAX_ORBIT_TILT;

		age.data = planet.age - (planet.age * fabs(RandDblFromLng(seeds.z)) * 0.1);
		moonDat = GetMoonData(type, planet.water_frac, seeds.z, seeds.w);
		moon.water_frac = moonDat.water;
		moon.atmos_spec = moonDat.color;
		habitability = moonDat.score;
		habitability += (64.0f-(fabs(fmin(2.0f,(moon.temp/EARTH_TEMP))-1.0f)*64.0f)) + fmin(64.0f,moon.water_frac*128.0f);
		hostility = fmin(255.0, (1.0 + (0.1 * RandDblFromLng(seeds.w))) * planet.hostility);
		
		seeds.x = RandomLong(seeds.w);
		seeds.y = RandomLong(seeds.x);
		seeds.z = RandomLong(seeds.y);
		seeds.w = RandomLong(seeds.z);

		if (lifeType < TYPE_0_CIV) {
			function = (fabs(RandDblFromLng(seeds.x)) < 0.05) ? 1 : 0;
			life_level = (seeds.y % habitability) * fmin(1.0f, age.data / EARTH_AGED);
		} else if (lifeType < TYPE_I_CIV) {
			function = seeds.x % 4;
			life_level = (function>0) ? planet.life_level : fmin((float)(seeds.y % (planet.life_level-64)),
						 (seeds.y % habitability) * fmin(1.0f, age.data / EARTH_AGED));
		} else {
			function = 4 + (seeds.x % 6);
			life_level = planet.life_level;
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
		
		species = (seeds.z % life_level) * LifeLvlToEnum(life_level) * 0.1f;
		variFrac = moon.gravity / MAX_GRAVITY;
		moon.atmos_radi = moon.radius * (1.2 - (0.15f * variFrac));
		moon.atmos_spec.red += 10.0f * (moon.temp/LAVA_MAX_TEMP);
		moon.atmos_spec.green += (5.0f * (habitability/255.0f)) + (5.0f * (species/(float)MAX_MILL_SPECIES));
		moon.atmos_spec.blue += (5.0f * fmax(0.0f, 1.0f-(moon.temp/BOILING_TEMP))) + (5.0f * moon.water_frac);
		moon.atmos_spec.alpha += (60.0f * variFrac) + (60.0f * (life_level/255.0f)) + (10.0f * RandDblFromLng(seeds.z));

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
__global float4* terrain_map, read_only image2d_t texture,
const uint moon_index, const uint star_index, 
const Planet planet, RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	Star star = star_buffer[star_index];
	Moon moon = moon_buffer[moon_index];
	float4 color;
	
	float3 starClr = ColorToVec3(star.color);
	float4 diffuse = moonDiffData[moon.type];
	float4 specular = moonSpecData[moon.type];

	starClr = fmin(starClr * 2.0f, (float3)(1.0f,1.0f,1.0f));

	double hitDist;
	float rotAngle;
	float fakeRad;
	float2 uv;
	float3 ltiVect;
	double3 surfPnt;
	double3 normRay;
	double3 primRay;
	
	float lightPow = STAR_LIGHT_POW * star.luminosity;
	float waveShift = 100.f + render_info.g_time * WAVE_SPEED;
	
	// loop through pixel fragments
	for (short rx=0; rx < render_info.aa_dim; ++rx)
	{	
		for (short ry=0; ry < render_info.aa_dim; ++ry)
		{
			// calculate primary ray for pixel fragment
			primRay = render_info.bl_ray + (render_info.cam_rgt * (pix_X+(render_info.aa_inc*rx)));
			primRay = normalize(primRay + (render_info.cam_up * (pix_Y+(render_info.aa_inc*ry))));
		
			// check if ray intersects with sphere
			fakeRad = moon.radius * PLANET_MAG;
			hitDist = RaySphereIntersect(render_info.cam_pos, primRay, moon.position, fakeRad*fakeRad);

			if (hitDist > 0.0) {
				// get hit point in world space
				surfPnt = render_info.cam_pos + (primRay * hitDist);
				// get normal to the intersection point and sphere
				normRay = normalize(surfPnt - moon.position);
				// calculate rotation angle based on time and rotation speed
				rotAngle = dfract(render_info.g_time/moon.rot_time);
				// calculate uv coordinates based on intersection
				uv = CalcTerrainMapUV(normRay, rotAngle);
				// get surface color for intersection point
				color = CalcSurfColor(texture, uv);
				// get adjusted normal based on height map
				normRay = (color.w == 1.0f || moon.temp <= FREEZING_TEMP) ? 
					CalcMoonNormal(terrain_map, uv, normRay, 0.2f, rotAngle) : 
					CalcWaterNormal(uv, waveShift, color.w, rotAngle);
				// check if planet is blocking light from star
				fakeRad = planet.radius * PLANET_MAG;
				primRay = normalize(star.position - surfPnt);
				hitDist = RaySphereIntersect(surfPnt, primRay, planet.position, fakeRad*fakeRad);
				// calc lighting/shading for intersected surface
				ltiVect = (hitDist > 0.0) ? ((float3)(0.02f,0.02f,0.02f) + diffuse.w) * starClr : 
					CalcSurfLight(surfPnt, normRay, render_info.cam_pos, star.position, starClr, diffuse, specular, lightPow);
				
				frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx] = Vec3ToColor(VecMin(ltiVect*color.xyz,1.0f));
			}
		}
	}
}

__kernel void DrawPlanet(__global Planet* planet_buffer, __global Moon* moon_buffer, __global Star* star_buffer,
__global PFrags* frag_buffer, __global float4* terrain_map, read_only image2d_t texture, const uint planet_index, 
const uint star_index, RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	Star star = star_buffer[star_index];
	Planet planet = planet_buffer[planet_index];
	Moon moon;
	
	float3 starClr = ColorToVec3(star.color);
	float4 diffuse = planetDiffData[planet.type];
	float4 specular = planetSpecData[planet.type];
	float4 color;
	RGB32 fragClr;
	
	starClr = fmin(starClr * 2.0f, (float3)(1.0f,1.0f,1.0f));

	float2 uv;
	float3 ltiVect;
	double3 surfPnt, normRay, primRay;
	float lightPow = STAR_LIGHT_POW * star.luminosity;
	
	float3 atmoLight = (float)fmin(1.0, lightPow/pow(distance(planet.position, star.position), 2.0)) * starClr;
	planet.atmos_spec = ColorMultiplyV3(planet.atmos_spec.rgba, atmoLight);
	float3 cloudCol = (float3)(1.0f,1.0f,1.0f);
	
	//float randFlt = (float)RandomLong(star_index*planet_index) / ULONG_MAX;
	double randDbl = (double)RandomLong(render_info.rand_int*pix_index) / ULONG_MAX;
	double pntDist = distance(planet.position, render_info.cam_pos);
	double hitDist = pntDist;
	uchar rayState = 0;
	uchar fragIndex;
	
	float moonRad2, atmosDist, accumVal, rotAngle;
	float fakeRad = planet.radius * PLANET_MAG;
	float fakeRad2 = fakeRad * fakeRad;
	float fakeAtmo = planet.atmos_radi * PLANET_MAG;
	float mulAccum = (planet.atmos_spec.alpha / 255.0f) * RAY_DENS_MUL;
	float waveShift = 100.f + render_info.g_time * WAVE_SPEED;
	float cloudShift = render_info.g_time * CLOUD_SPEED;
	float cloudHide = (1.f - planet.water_frac) * 0.5f;

	// get primary ray for current pixel
	primRay = render_info.bl_ray + (render_info.cam_rgt * ((float)pix_X+render_info.aa_div));
	primRay = normalize(primRay + (render_info.cam_up * ((float)pix_Y+render_info.aa_div)));
	
	// check if camera is outside atmosphere
	if (pntDist > fakeAtmo) {
		// check if ray intersects with atmosphere
		hitDist = RaySphereIntersect(render_info.cam_pos, primRay, planet.position, fakeAtmo*fakeAtmo);
		rayState = 1;
	}

	if (hitDist > 0.0) {
		// calculate intersection point on/in atmosphere
		surfPnt = (rayState == 0) ? render_info.cam_pos : render_info.cam_pos + (primRay * hitDist);
		// calculate ray step distance based on atmosphere size
		atmosDist = fakeAtmo - fakeRad;
		primRay *= atmosDist / RAY_STEP_DIV;
		// shift starting point forward by small random distance
		surfPnt += primRay * (0.01 + randDbl);
		// get distance between intersection point and planet center
		pntDist = distance(planet.position, surfPnt);
		accumVal = 0.0f;
		rayState = 2;
		
		while (pntDist < fakeAtmo && pntDist > fakeRad)
		{
			// check if star light can reach current point inside atmosphere
			if (RaySphereIntersect(surfPnt, normalize(star.position-surfPnt), planet.position, fakeRad2) == 0.0) {
				// check if moons are blocking light from star
				hitDist = 0.0;
				for (uchar m=0; m < planet.moons; ++m) {
					moon = moon_buffer[m];
					moonRad2 = pow(moon.radius * PLANET_MAG, 2.0f);
					normRay = normalize(star.position - surfPnt);
					hitDist = RaySphereIntersect(surfPnt, normRay, moon.position, moonRad2);
					if (hitDist > 0.0) break;
				}
				accumVal += (hitDist > 0.0) ? 0.0f : mulAccum * pow(1.0f - ((float)(pntDist-fakeRad) / atmosDist), ATMO_DIST_POW);
				if (accumVal >= 1.0f) break;
			}
			surfPnt += primRay;
			pntDist = distance(planet.position, surfPnt);
		}

		planet.atmos_spec.alpha = ATMO_B_ALPHA * accumVal;
	}
		
	// loop through pixel fragments
	for (short rx=0; rx < render_info.aa_dim; ++rx)
	{
		for (short ry=0; ry < render_info.aa_dim; ++ry)
		{
			// calculate primary ray for pixel fragment
			primRay = render_info.bl_ray + (render_info.cam_rgt * (pix_X+(render_info.aa_inc*rx)));
			primRay = normalize(primRay + (render_info.cam_up * (pix_Y+(render_info.aa_inc*ry))));
		
			// check if ray intersects with sphere
			hitDist = RaySphereIntersect(render_info.cam_pos, primRay, planet.position, fakeRad2);

			if (hitDist > 0.0) {
				// get hit point in world space
				surfPnt = render_info.cam_pos + (primRay * hitDist);
				// get normal to the intersection point and sphere
				normRay = normalize(surfPnt - planet.position);
				// calculate rotation angle based on time and rotation speed
				rotAngle = dfract(render_info.g_time/planet.rot_time);
				// calculate uv coordinates based on intersection
				uv = CalcTerrainMapUV(normRay, rotAngle);
				// get surface color for intersection point
				color = CalcSurfColor(texture, uv);
				// calculate amount of cloud cover over terrain
				accumVal = CalcCloudCover(normRay, cloudShift, cloudHide, rotAngle);
				// get adjusted normal based on height map
				normRay = (color.w == 1.0f || planet.temp <= FREEZING_TEMP) ?
					CalcPlanetNormal(terrain_map, uv, normRay, 0.3f, rotAngle) : 
					CalcWaterNormal(uv, waveShift, color.w, rotAngle);
					
				// check if moons are casting shade on surface point
				hitDist = 0.0;
				for (uchar m=0; m < planet.moons; ++m) {
					moon = moon_buffer[m];
					moonRad2 = pow(moon.radius * PLANET_MAG, 2.0f);
					primRay = normalize(star.position - surfPnt);
					hitDist = RaySphereIntersect(surfPnt, primRay, moon.position, moonRad2);
					if (hitDist > 0.0) break;
				}
				
				// calc lighting/shading for intersected surface
				ltiVect = (hitDist > 0.0) ? ((float3)(0.05f,0.05f,0.05f) + diffuse.w) * starClr : 
					CalcSurfLight(surfPnt, normRay, render_info.cam_pos, star.position, starClr, diffuse, specular, lightPow);
					
				// blend cloud color over terrain/water and apply lighting
				color.xyz = VecMin(ltiVect * mix(color.xyz, cloudCol, accumVal), 1.f);

				// blend atmosphere color over final color and save to fragment
				frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx] = 
					AlphaBlend(planet.atmos_spec, Vec3ToColor(color.xyz));
				
			} else if (rayState == 2) {
				fragIndex = (ry * render_info.aa_dim) + rx;
				fragClr = frag_buffer[pix_index].colors[fragIndex];
				if (fragClr.alpha > 0) {
					frag_buffer[pix_index].colors[fragIndex] = AlphaBlend(planet.atmos_spec, fragClr);
				} else {
					frag_buffer[pix_index].colors[fragIndex] = planet.atmos_spec;
				}
			}
		}
	}
}

__kernel void DrawStar(__global Star* star_buffer, __global PFrags* frag_buffer,
const uint star_index, const double game_time, RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;

	UCharVec8 starColDat;
	Star star = star_buffer[star_index];
	starColDat.data = starAvgColors[star.type];
	uchar starGlow = starColDat.s[6];
	uchar starShadow = starColDat.s[7];

	double sDist;
	float fakeRad;
	double3 wldPnt;
	//double3 camPnt;
	double3 normRay;
	double3 primRay;
	int cSum;
	
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
				// get hit point in world space
				wldPnt = render_info.cam_pos + (primRay * sDist);

				normRay = normalize(wldPnt - star.position);
				
				cSum = ((noise((double4)(floor(normRay*500.0),game_time)) + 
						noise((double4)(floor(normRay*250.0),game_time))) * starGlow);

				frag_buffer[pix_index].colors[(ry * render_info.aa_dim) + rx].rgba = (uchar4)
					(min(max((int)star.color.red+cSum-starShadow, 0), 255),
					 min(max((int)star.color.green+cSum-starShadow, 0), 255),
					 min(max((int)star.color.blue+cSum-starShadow, 0), 255), 255);
			}
		}
	}
}

__kernel void DrawStars(write_only image2d_t pix_buffer, __global InstanceData* idata_buffer,
__global Star* star_buffer, __global uint* adb_sizes, __global uint* loaded_star, RenderInfo render_info)
{
    uint zone_X = get_global_id(0);
	uint zone_Y = get_global_id(1);
	uint zone_Z = get_global_id(2);
	uint zone_index = zone_X + SECTOR_SPAN * (zone_Y + SECTOR_SPAN * zone_Z);
	
	Star star = star_buffer[zone_index];
	RGB32 color;
	double3 sprc;
	int2 pixelCoords;
	int2 ptempCoords;
	float2 screenCoords;
	float2 colFracs;
	float4 normColor;
	double starDist;//, randFrac;
	float starRadp, distFrac;
	
	sprc = VectRot(star.position - render_info.cam_pos, render_info.cam_ori);
	
	if (sprc.z <= 0.0f) return;
	
	// project star position onto screen
	screenCoords.x = render_info.half_X + (sprc.x / sprc.z) * render_info.cam_foc;
	screenCoords.y = render_info.half_Y + (sprc.y / sprc.z) * render_info.cam_foc;
	
	pixelCoords.x = screenCoords.x;
	pixelCoords.y = screenCoords.y;

	if (pixelCoords.x < 0 || pixelCoords.y < 0 || 
		pixelCoords.x >= render_info.pixels_X || 
		pixelCoords.y >= render_info.pixels_Y) return;

	starDist = length(sprc);
	distFrac = fmin((float)(starDist / SECTOR_HALF), 1.0f);
	starRadp = StarGlowSize(star.luminosity, starDist);
	
	// simulate drop in saturation and visibility with distance
	star.color.alpha = round(distFrac*255.0f);
	color = AlphaBlendStar(star.color, 250);

	// check if star glow radius less than half a pixel
	if (starRadp < 0.5f) {
		// set color transparency based on distance
		color.alpha = round(130.0f * (1.0f - distFrac));
	
		// write star color to corresponding pixel
		normColor = ColorToVec4(ScaleColor(color, starRadp+0.3f));
		write_imagef(pix_buffer, pixelCoords, normColor);

		// color nearby pixels based on position and size of star
		screenCoords.x -= (float)pixelCoords.x;
		screenCoords.y -= (float)pixelCoords.y;
		colFracs.x = .1f + fabs(screenCoords.x - .5f);
		colFracs.y = .1f + fabs(screenCoords.y - .5f);

		ptempCoords.x = screenCoords.x<.5f ? pixelCoords.x-1 : pixelCoords.x+1;
		ptempCoords.y = screenCoords.y<.5f ? pixelCoords.y-1 : pixelCoords.y+1;
		ptempCoords.x = min(max(0, ptempCoords.x), (int)render_info.pixels_X-1);
		ptempCoords.y = min(max(0, ptempCoords.y), (int)render_info.pixels_Y-1);
		
		write_imagef(pix_buffer, (int2)(ptempCoords.x, pixelCoords.y), normColor*colFracs.x);
		write_imagef(pix_buffer, (int2)(pixelCoords.x, ptempCoords.y), normColor*colFracs.y);
		write_imagef(pix_buffer, ptempCoords, normColor*((colFracs.x+colFracs.y)*.33f));

		// chance to increase pixel brightness to create twinkling effect
		/*if ((zone_index^render_info.rand_int)%89 == 1) {
			randFrac = sin(render_info.g_time*render_info.rand_int*zone_index);
			color = ScaleColor(color, fabs(randFrac));
			write_imagef(pix_buffer, pixelCoords, ColorToVec4(color));
		}*/
		
	} else {
		// set color transparency based on distance
		color.alpha = round(255.0f * (1.0f - distFrac));
		// create depth index [0-9]
		uint depth_index = floor(distFrac*9.9999f);
		// create instance data structure
		InstanceData idata = {
			ColorToVec4(color),
			(float2)(screenCoords.x, screenCoords.y),
			starRadp * STAR_TEX_RATIO, distFrac
		};
		// save instance data to buffer (into 10 groups separated by depth)
		idata_buffer[increment(&adb_sizes[depth_index])+(IBUFF_OFFSET*depth_index)] = idata;
		
		// should save index of closest star
		if (distFrac < FLARE_VIS_DEPTH) {
			*loaded_star = zone_index;
		}
	}
}

__kernel void MousePickPlanet(__global Planet* planet_buffer, __global Moon* moon_buffer,
__global volatile uint* min_dist, __global int* planet_found, const float2 mouse_pos, RenderInfo render_info)
{
	uint planet_index = get_global_id(0);
	Planet planet = planet_buffer[planet_index];

	double3 mouseRay = render_info.bl_ray + (render_info.cam_rgt * (double3)(mouse_pos.x,mouse_pos.x,mouse_pos.x));
	mouseRay = normalize(mouseRay + (render_info.cam_up * (double3)(mouse_pos.y,mouse_pos.y,mouse_pos.y)));
	
	float fakeRad = planet.radius * PLANET_MAG;
	double sDist = RaySphereIntersect(render_info.cam_pos, mouseRay, planet.position, fakeRad*fakeRad);
	uint fltInt, moon_index;
	uint mstart = planet_index * MAX_MOONS;
	
	if (sDist > 0.0) {
		fltInt = sDist * 0.0001;
		atomic_min(min_dist, fltInt);
		//barrier(CLK_GLOBAL_MEM_FENCE);
		if (*min_dist == fltInt) {
			planet_found[0] = planet_index;
		}
		//return;
	}
	
	//barrier(CLK_GLOBAL_MEM_FENCE);
	
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
__global int* star_found, const float2 mouse_pos, RenderInfo render_info)
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
		
		p_dist = distance(mouse_pos, screen_coords);
		s_prad = StarGlowSize(star.luminosity, length(sprc));
		fltInt = s_prad * 10000.0f;

		if (p_dist < s_prad+5.0f) {
			atomic_min(min_dist, fltInt);
			//barrier(CLK_GLOBAL_MEM_FENCE);
			if (*min_dist == fltInt) {
				*star_found = zone_index;
			}
		}
	}
}

__kernel void FillFragBuff(__global PFrags* frag_buffer, RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	
	frag_buffer[(pix_Y * render_info.pixels_X) + pix_X].data = (uint4)(0,0,0,0);

	/*uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	uint pix_seed = render_info.rand_int * pix_index;

	RGB32 noiseClr;
	ulong xseed = RandomLong(pix_seed * pix_X);
	ulong yseed = RandomLong(pix_seed * pix_Y);
	uchar noiseMag = (xseed & yseed) % NOISE_MAX;
	noiseClr.rgba = (uchar4)(noiseMag, noiseMag, noiseMag, 0);
	
	frag_buffer[pix_index].data = (uint4)(noiseClr.data,noiseClr.data,noiseClr.data,noiseClr.data);*/
}

__kernel void FragsToFrame(__global PFrags* frag_buffer,
write_only image2d_t pix_buffer, RenderInfo render_info)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint pix_index = (pix_Y * render_info.pixels_X) + pix_X;
	
	PFrags frags = frag_buffer[pix_index];
	float4 sumColor = (float4)(0.0f,0.0f,0.0f,0.0f);
	float alphaFlt;
		
	for (uint r=0; r < render_info.aa_lvl; ++r) {
		alphaFlt = frags.colors[r].alpha / 255.0f;
		sumColor.x += alphaFlt * frags.colors[r].red;
		sumColor.y += alphaFlt * frags.colors[r].green;
		sumColor.z += alphaFlt * frags.colors[r].blue;
		sumColor.w += alphaFlt;
	}
	
	if (sumColor.w > 0.0f) {
		sumColor *= render_info.aa_div;
		write_imagef(pix_buffer, (int2)(pix_X, pix_Y), (float4)(sumColor.xyz / 255.0f, sumColor.w));
	}
}

__kernel void GenNormalMap(__global float4* terrain_map, __global float4* temp_normals)
{
    int pix_X = get_global_id(0);
	int pix_Y = get_global_id(1);
	int width = get_global_size(0);
	int height = get_global_size(1);
	uint index_Y = pix_Y * width;
	uint pix_index = index_Y + pix_X;

	float halfHeight = height * 0.5f;
	float yDist = (halfHeight - (pix_Y+0.5f)) / halfHeight;
	int xStep = round(1.f / sqrt(1.f - (yDist*yDist)));
	
	uint LX = ((pix_X - xStep) < 0) ? width - xStep : pix_X - xStep;
	uint RX = (uint)(pix_X + xStep) % width;
	uint BY = ((pix_Y - 1) < 0) ? height - 1 : pix_Y - 1;
	uint TY = (pix_Y + 1) % height;
	
	float4 N = terrain_map[pix_index];
	float4 L = terrain_map[index_Y + LX];
	float4 R = terrain_map[index_Y + RX];
	float4 T = terrain_map[(TY * width) + pix_X];
	float4 B = terrain_map[(BY * width) + pix_X];
	
	float LR = (L.w - R.w) * 10000.f;
	float BT = (B.w - T.w) * 10000.f;
	float3 diff = (float3)(0.f,0.f,0.f);
	
	diff = LR < 0.f ? (L.xyz - N.xyz) * (float3)(-LR,-LR,-LR) :
			(R.xyz - N.xyz) * (float3)(LR,LR,LR);
	
	diff += BT < 0.f ? (B.xyz - N.xyz) * (float3)(-BT,-BT,-BT) :
			(T.xyz - N.xyz) * (float3)(BT,BT,BT);
	
	temp_normals[pix_index] = (float4)(normalize(N.xyz + diff), N.w);
}

__kernel void GenHeightMap(__global float4* terrain_map,  
const float rand_flt, const float mount_mix, const uchar is_planet)
{
    uint pix_X = get_global_id(0);
	uint pix_Y = get_global_id(1);
	uint width = get_global_size(0);
	uint height = get_global_size(1);
	uint pix_index = (pix_Y * width) + pix_X;
	float2 uv = (float2)((pix_X+0.5f)/width, (pix_Y+0.5f)/height);
	
	float3 n = SphereNormFromUV(uv);
	float3 v = (float3)(n.x+rand_flt, n.y+rand_flt, n.z+rand_flt);
	
	float mapHeight = (is_planet == 1) ?
		mix(PerlinFbm(v, 5), pow(WorleyFbm(v, 5), 2.0f), mount_mix) : 
		1.f - CraterFbm(v, 5);
	
	terrain_map[pix_index] = (float4)(n, mapHeight);
}

/*__kernel void GenWaterMap(__global float4* water_map, 
const float rand_flt, const float scale, const uchar detail)
{
    int pix_X = get_global_id(0);
	int pix_Y = get_global_id(1);
	int width = get_global_size(0);
	int height = get_global_size(1);
	uint pix_index = (pix_Y * width) + pix_X;
	float2 uv = (float2)((pix_X+0.5f)/width, (pix_Y+0.5f)/height);
	
	float halfHeight = height * 0.5f;
	float yDist = (halfHeight - (pix_Y+0.5f)) / halfHeight;
	int xStep = round(1.f / sqrt(1.f - (yDist*yDist)));
	
	uint LX = ((pix_X - xStep) < 0) ? width - xStep : pix_X - xStep;
	uint RX = (uint)(pix_X + xStep) % width;
	uint BY = ((pix_Y - 1) < 0) ? height - 1 : pix_Y - 1;
	uint TY = (pix_Y + 1) % height;

	float3 N = SphereNormFromUV(uv);
	float3 L = SphereNormFromUV((float2)((LX+0.5f)/width, uv.y));
	float3 R = SphereNormFromUV((float2)((RX+0.5f)/width, uv.y));
	float3 B = SphereNormFromUV((float2)(uv.x, (BY+0.5f)/height));
	float3 T = SphereNormFromUV((float2)(uv.x, (TY+0.5f)/height));
	
	float3 vN = (float3)(N.x*scale+rand_flt, N.y*scale+rand_flt, N.z*scale+rand_flt);
	float3 vL = (float3)(L.x*scale+rand_flt, L.y*scale+rand_flt, L.z*scale+rand_flt);
	float3 vR = (float3)(R.x*scale+rand_flt, R.y*scale+rand_flt, R.z*scale+rand_flt);
	float3 vB = (float3)(B.x*scale+rand_flt, B.y*scale+rand_flt, B.z*scale+rand_flt);
	float3 vT = (float3)(T.x*scale+rand_flt, T.y*scale+rand_flt, T.z*scale+rand_flt);
	
	float LR = (WorleyFbm(vL, detail) - WorleyFbm(vR, detail)) * 10000.f;
	float BT = (WorleyFbm(vB, detail) - WorleyFbm(vT, detail)) * 10000.f;
	float3 diff = (float3)(0.f,0.f,0.f);
	
	diff = LR < 0.f ? (L - N) * (float3)(-LR,-LR,-LR) :
			(R - N) * (float3)(LR,LR,LR);
	
	diff += BT < 0.f ? (B - N) * (float3)(-BT,-BT,-BT) :
			(T - N) * (float3)(BT,BT,BT);
			
	water_map[pix_index] = (float4)(normalize(N + diff), WorleyFbm(vN, detail));
}*/
