#pragma once
#include "CLTypes.h"
#include "ReadWrite.h"

namespace GLOBALS {
	const cl_AAInfo AA_X1  {1.0,  1.0,   1,  1};
	const cl_AAInfo AA_X4  {0.5,  0.25,  4,  2};
	const cl_AAInfo AA_X9  {1./3, 1./6,  9,  3};
	const cl_AAInfo AA_X16 {0.25, 0.125, 16, 4};
}

namespace SCREEN {

    struct Resolution {
        uint32_t Width;
        uint32_t Height;
        uint8_t BlocksX;
        uint8_t BlocksY;
        float AspectRatio;
    };

    static const float ratio_5x4   = 5.0f / 4.0f; //1.25
    static const float ratio_4x3   = 4.0f / 3.0f; //1.33
    static const float ratio_15x10 = 15.0f / 10.0f; //1.5
    static const float ratio_16x10 = 16.0f / 10.0f; //1.6
    static const float ratio_15x9  = 15.0f / 9.0f; //1.66
    static const float ratio_16x9  = 16.0f / 9.0f; //1.77
    static const float ratio_21x9  = 21.0f / 9.0f; //2.33

    static const uint32_t RA_SIZE = 57;
    static Resolution res_array[RA_SIZE] =
    {
        {7680, 4800, 80, 80, ratio_16x10},
        {7680, 4320, 80, 80, ratio_16x9},
        {6400, 4800, 64, 64, ratio_4x3},
        {5120, 4096, 64, 64, ratio_5x4},
        {5120, 3200, 64, 64, ratio_16x10},
        {5120, 2880, 64, 64, ratio_16x9},
        {4096, 3072, 64, 64, ratio_4x3},
        {3840, 2400, 40, 40, ratio_16x10},
        {3840, 2160, 40, 40, ratio_16x9},
        {3200, 2400, 40, 40, ratio_4x3},
        {3200, 1800, 40, 40, ratio_16x9},
        {2880, 1800, 40, 40, ratio_16x10},
        {2560, 2048, 32, 32, ratio_5x4},
        {2560, 1600, 32, 32, ratio_16x10},
        {2560, 1440, 32, 32, ratio_16x9},
        {2048, 1536, 32, 32, ratio_4x3},
        {2048, 1152, 32, 32, ratio_16x9},
        {1920, 1200, 20, 20, ratio_16x10},
        {1920, 1080, 20, 20, ratio_16x9},
        {1680, 1050, 21, 21, ratio_16x10},
        {1600, 1200, 20, 20, ratio_4x3},
        {1600, 900,  20, 20, ratio_16x9},
        {1440, 900,  20, 20, ratio_16x10},
        {1440, 960,  20, 20, ratio_15x10},
        {1400, 1050, 20, 21, ratio_4x3},
    //  {1366, 768,  2,  32, ratio_16x9},
        {1360, 1024, 16, 16, ratio_4x3},
        {1360, 768,  16, 16, ratio_16x9},
    //  {1344, 768,  16, 16, ratio_7x4},
        {1280, 1024, 16, 16, ratio_5x4},
        {1280, 960,  16, 16, ratio_4x3},
        {1280, 800,  16, 16, ratio_16x10},
        {1280, 768,  16, 16, ratio_15x9},
        {1280, 720,  16, 16, ratio_16x9},
        {1152, 864,  16, 16, ratio_4x3},
        {1152, 768,  16, 16, ratio_15x10},
        {1024, 768,  16, 16, ratio_4x3},
        {1024, 576,  16, 16, ratio_16x9},
        {960,  640,  10, 10, ratio_15x10},
        {960,  540,  10, 10, ratio_16x9},
        {800,  600,  10, 10, ratio_4x3},
        {800,  480,  10, 10, ratio_15x9},
        {800,  450,  10, 10, ratio_16x9},
        {768,  480,  8,  8,  ratio_16x10},
        {720,  480,  8,  8,  ratio_15x10},
        {640,  480,  8,  8,  ratio_4x3},
        {640,  384,  8,  8,  ratio_15x9},
        {640,  360,  8,  8,  ratio_16x9},
        {480,  360,  8,  8,  ratio_4x3},
        {480,  320,  8,  8,  ratio_15x10},
        {480,  272,  8,  8,  ratio_16x9},
        {432,  240,  8,  8,  ratio_16x9},
        {400,  240,  8,  8,  ratio_15x9},
        {384,  240,  6,  6,  ratio_16x10},
        {360,  240,  5,  5,  ratio_15x10},
        {320,  240,  5,  5,  ratio_4x3},
        {256,  160,  4,  4,  ratio_16x10},
        {240,  160,  4,  4,  ratio_15x10},
        {160,  120,  4,  4,  ratio_4x3}
    };

    inline Resolution GetProfile(uint32_t width, uint32_t height)
    {
        for (uint32_t i=0; i < RA_SIZE; ++i)
        {
            Resolution& res = res_array[i];

            if (res.Width == width && res.Height == height) {
                return res;
            }
        }

        PrintLine("Unsupported resolution detected: "+VarToStr(width)+"x"+VarToStr(height));
        exit(EXIT_FAILURE);
    }
};
