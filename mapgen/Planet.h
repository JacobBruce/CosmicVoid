/* Planet.h */
/* planet map generating program */
/* Copyright 1988--2020 Torben AE. Mogensen */
/* Modified by J.D. Bruce for Cosmic Void */

/* Generates planet maps based on recursive spatial subdivision of a tetrahedron containing the globe. */

/* NOTE: different machine precision can cause the same seed numbers to yield very different planets. */

#pragma once
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "../Colors.h"
#include "../MathExt.h"
#include "../CLTypes.h"

#define VOLC_RGB    117,101,45
#define LAVA_RGB    162,100,23
#define ICE_RGB     216,225,227
#define WATER_RGB   25,59,94
#define DWATER_RGB  27,41,71
#define DIRT_RGB    77,72,50
#define TREES_RGB   70,89,48
#define ROCK_RGB    113,117,108
#define SAND_RGB	123,125,105
#define GAS_RGB     202,208,162
#define IRON_RGB    135,113,26
#define CARBON_RGB  60,48,30
#define DESERT_RGB  168,162,130
#define SILIC_RGB   152,158,160

class TerrainGen {
private:

	int Width, Height;

public:

    TerrainGen(int width, int height) : Width(width), Height(height) {}

    uint8_t PlanetLandBlendAlpha(cl_uchar type) {
        static uint8_t alphas[12] = {75,25,25,60,60,50,50,60,50,100,150,150};
        if (type > 11) return 50;
        return alphas[type];
    }

    uint8_t MoonLandBlendAlpha(cl_uchar type) {
        static uint8_t alphas[4] = {100,60,50,50};
        if (type > 3) return 50;
        return alphas[type];
    }

    float DrawPlanetColors(float4* heights, RGB32* texture, const cl_Planet& planet)
    {
        RGB32 landColor(DIRT_RGB, 255);
        RGB32 waterColor(WATER_RGB, 255);
        RGB32 dwaterColor(DWATER_RGB, 255);
        RGB32 sandColor(SAND_RGB, 255);
        RGB32 rockColor(ROCK_RGB, 255);
        RGB32 desertColor(DESERT_RGB, 255);
        RGB32 snowColor(ICE_RGB, 255);
        RGB32 color;

        float tempDiff = EARTH_TEMP - planet.temp;
        float freezeFrac = std::min(std::max(tempDiff,0.f) / 50.f, 1.f);
        float frozenFrac = planet.water_frac * freezeFrac;
        float boilFrac = std::min(fabs(std::min(tempDiff,0.f)) / (LAVA_MIN_TEMP-BOILING_TEMP), 1.f);
        float densFrac = 1.f - std::min(1.f, (planet.density / EARTH_DENS) * 0.5f);

        float lowLevel = -0.1;
        float sandLevel = -0.01;
        float seaLevel = 0.0;
        float landLevel = 0.005;
        float rockLevel = 0.1f - (frozenFrac * 0.09f);
        float snowLevel = (0.15f - (frozenFrac * 0.1f)) + (boilFrac * 0.6f);
        float baseHeight = planet.water_frac * -0.1f;

        uint32_t waterPixs = 0;
        uint32_t pixCount = Width*Height;

        switch (planet.type) {
            case 0:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 1:
                landColor = AlphaBlend(RGB32(GAS_RGB, 255), RGB32(DESERT_RGB, planet.habitability)); break;
            case 2:
                landColor = AlphaBlend(RGB32(GAS_RGB, 255), RGB32(DESERT_RGB, planet.habitability)); break;
            case 3:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 4:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 5:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(TREES_RGB, planet.habitability)); break;
            case 6:
                landColor = AlphaBlend(RGB32(DIRT_RGB, 255), RGB32(TREES_RGB, planet.habitability)); break;
            case 7:
                landColor = AlphaBlend(RGB32(DESERT_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 8:
                landColor = AlphaBlend(RGB32(SILIC_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 9:
                landColor = AlphaBlend(RGB32(CARBON_RGB, 255), RGB32(DIRT_RGB, planet.habitability*densFrac)); break;
            case 10:
                landColor = AlphaBlend(RGB32(CARBON_RGB, 255), RGB32(DIRT_RGB, planet.habitability*densFrac)); break;
            case 11:
                landColor = AlphaBlend(RGB32(IRON_RGB, 255), RGB32(DIRT_RGB, planet.habitability*densFrac)); break;
        }

        landColor.alpha = 50;
        dwaterColor = AlphaBlend(dwaterColor, landColor);
        waterColor = AlphaBlend(waterColor, landColor);
        landColor.alpha = PlanetLandBlendAlpha(planet.type);
        sandColor = AlphaBlend(sandColor, landColor);
        rockColor = AlphaBlend(rockColor, landColor);
        desertColor = AlphaBlend(desertColor, landColor);

        if (planet.type == 9) {// lava planet
            waterColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*160.f)));
            dwaterColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*240.f)));
            landColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
            rockColor = AlphaBlend(rockColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
            sandColor = AlphaBlend(sandColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
            desertColor = AlphaBlend(desertColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
        } else {
            snowColor.alpha = std::round(160.f*freezeFrac);
            waterColor = AlphaBlend(waterColor, snowColor);
            dwaterColor = AlphaBlend(dwaterColor, snowColor);
            snowColor.alpha = std::round(240.f*frozenFrac);
            landColor = AlphaBlend(landColor, snowColor);
            sandColor = AlphaBlend(sandColor, snowColor);
            rockColor = AlphaBlend(rockColor, snowColor);
            desertColor = AlphaBlend(desertColor, snowColor);
        }

        for (uint32_t pixIndex=0; pixIndex<pixCount; ++pixIndex)
        {
            float height = heights[pixIndex].w + baseHeight;
            if (height < sandLevel) {
                height = smoothstep(lowLevel, sandLevel, height);
                waterColor.alpha = std::round(255.0f * height);
                color = AlphaBlend(dwaterColor, waterColor);
                color.alpha = 74.0f + (180.0f * (1.0f-height));
                waterPixs++;
            } else if (height < seaLevel) {
                height = smoothstep(sandLevel, seaLevel, height);
                sandColor.alpha = std::round(255.0f * height);
                color = AlphaBlend(waterColor, sandColor);
                color.alpha = 74.0f * (1.0f-height);
                waterPixs++;
            } else if (height < landLevel) {
                landColor.alpha = std::round(255.0f * smoothstep(seaLevel, landLevel, height));
                color = AlphaBlend(sandColor, landColor);
            } else if (height < rockLevel) {
                rockColor.alpha = std::round(255.0f * smoothstep(landLevel, rockLevel, height));
                color = AlphaBlend(landColor, rockColor);
            } else if (height < snowLevel) {
                desertColor.alpha = std::round(255.0f * smoothstep(rockLevel, snowLevel, height));
                color = AlphaBlend(rockColor, desertColor);
            } else {
                snowColor.alpha = std::round(255.0f * smoothstep(snowLevel, 1.0f, height));
                color = AlphaBlend(desertColor, snowColor);
            }

            texture[pixIndex] = color;
        }

        float waterFrac = (float)waterPixs / (Width*Height);
        return (waterFrac + planet.water_frac) * 0.5f;
    }

    float DrawMoonColors(float4* heights, RGB32* texture, const cl_Moon& moon)
    {
        RGB32 landColor(SILIC_RGB, 255);
        RGB32 dwaterColor(DWATER_RGB, 255);
        RGB32 waterColor(WATER_RGB, 255);
        RGB32 rockColor(ROCK_RGB, 255);
        RGB32 desertColor(DESERT_RGB, 255);
        RGB32 snowColor(ICE_RGB, 255);
        RGB32 color;

        float tempDiff = EARTH_TEMP - moon.temp;
        float freezeFrac = std::min(std::max(tempDiff,0.f) / 50.f, 1.f);
        float frozenFrac = moon.water_frac * freezeFrac;
        float boilFrac = std::min(fabs(std::min(tempDiff,0.f)) / (LAVA_MIN_TEMP-BOILING_TEMP), 1.f);
        float densFrac = 1.f - std::min(1.f, (moon.density / EARTH_DENS) * 0.5f);

        float lowLevel = -0.4;
        float seaLevel = 0.0;
        float landLevel = 0.5f;
        float rockLevel = 0.7f;
        float snowLevel = (0.9f - (frozenFrac * 0.1f)) + (boilFrac * 0.1f);
        float baseHeight = moon.water_frac * -0.4f;

        uint32_t waterPixs = 0;
        uint32_t pixCount = Width*Height;

        switch (moon.type) {
            case 0:
                landColor = AlphaBlend(RGB32(CARBON_RGB, 255), RGB32(DIRT_RGB, moon.habitability*densFrac)); break;
            case 1:
                landColor = AlphaBlend(RGB32(SILIC_RGB, 255), RGB32(DIRT_RGB, moon.habitability)); break;
            case 2:
                landColor = AlphaBlend(RGB32(SILIC_RGB, 255), RGB32(TREES_RGB, moon.habitability)); break;
            case 3:
                landColor = AlphaBlend(RGB32(SILIC_RGB, 255), RGB32(ROCK_RGB, moon.habitability)); break;
        }

        landColor.alpha = 50;
        dwaterColor = AlphaBlend(dwaterColor, landColor);
        waterColor = AlphaBlend(waterColor, landColor);
        landColor.alpha = MoonLandBlendAlpha(moon.type);
        rockColor = AlphaBlend(rockColor, landColor);
        desertColor = AlphaBlend(desertColor, landColor);

        if (moon.type > 0) {
            snowColor.alpha = std::round(160.f*freezeFrac);
            waterColor = AlphaBlend(waterColor, snowColor);
            dwaterColor = AlphaBlend(dwaterColor, snowColor);
            snowColor.alpha = std::round(240.f*frozenFrac);
            landColor = AlphaBlend(landColor, snowColor);
            rockColor = AlphaBlend(rockColor, snowColor);
            desertColor = AlphaBlend(desertColor, snowColor);
        } else {
            // volcanic moon
            waterColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*160.0f)));
            dwaterColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*240.0f)));
            landColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
            rockColor = AlphaBlend(rockColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
            desertColor = AlphaBlend(desertColor, RGB32(LAVA_RGB, std::round(boilFrac*100.f)));
        }

        for (uint32_t pixIndex=0; pixIndex<pixCount; ++pixIndex)
        {
            float height = heights[pixIndex].w + baseHeight;
            if (height < seaLevel) {
                height = smoothstep(lowLevel, seaLevel, height);
                waterColor.alpha = std::round(255.0f * height);
                color = AlphaBlend(dwaterColor, waterColor);
                color.alpha = 254.0f * (1.0f-height);
                waterPixs++;
            } else if (height < landLevel) {
                landColor.alpha = std::round(255.0f * smoothstep(seaLevel, landLevel, height));
                color.alpha = std::min(254.0f, std::abs(landLevel-height)*100.0f);
                color = AlphaBlend(waterColor, landColor);
            } else if (height < rockLevel) {
                rockColor.alpha = std::round(255.0f * smoothstep(landLevel, rockLevel, height));
                color = AlphaBlend(landColor, rockColor);
            } else if (height < snowLevel) {
                desertColor.alpha = std::round(255.0f * smoothstep(rockLevel, snowLevel, height));
                color = AlphaBlend(rockColor, desertColor);
            } else {
                snowColor.alpha = std::round(255.0f * smoothstep(snowLevel, 1.0f, height));
                color = AlphaBlend(desertColor, snowColor);
            }

            texture[pixIndex] = color;
        }

        float waterFrac = (float)waterPixs / (Width*Height);
        return (waterFrac + moon.water_frac) * 0.5f;
    }

};
