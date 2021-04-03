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
#define LAVA_RGB    158,98,25
#define ICE_RGB     237,245,247
#define WATER_RGB   37,118,153
#define DWATER_RGB  23,64,115
#define DIRT_RGB    97,95,54
#define EARTH_RGB   90,117,59
#define ROCK_RGB    129,138,117
#define SAND_RGB	239,240,223
#define GAS_RGB     202,208,162
#define IRON_RGB    150,124,20
#define CARBON_RGB  60,48,30
#define DESERT_RGB  155,150,80
#define SILIC_RGB   152,158,160

class MapGen {
private:

	double M  = -.02;   /* initial altitude (slightly below sea level) */
	double dd1 = 0.45;  /* weight for altitude difference */
	double POWA = 1.0; /* power for altitude difference */
	double dd2 = 0.035; /* weight for distance */
	double POW = 0.47;  /* power for distance function */

	//std::chrono::microseconds gsleep, csleep;

	int Depth, Width, Height;
	double r1,r2,r3,r4;
	double scale, rseed;

	double** heights;

	typedef struct Vertex
	{
	  double h; /* altitude */
	  double s; /* seed */
	  double x,y,z; /* coordinates */
	} vertex;

	/* For the vertices of the tetrahedron */
	vertex tetra[4];

	vertex ssa, ssb, ssc, ssd;

	/* distance squared between vertices */
	static double dist2(vertex a, vertex b)
	{
	  double abx, aby, abz;
	  abx = a.x-b.x; aby = a.y-b.y; abz = a.z-b.z;
	  return abx*abx+aby*aby+abz*abz;
	}

public:

    //MapGen(size_t gen_delay, size_t col_delay) : gsleep(gen_delay), csleep(col_delay) {}

    /* random number generator taking two seeds */
	static double rand2(double p, double q)
	{
	  double r;
	  r = (p+3.14159265)*(q+3.14159265);
	  return(2.*(r-(int)r)-1.);
	}

	static double log_2(double x) { return(log(x)/log(2.0)); }

    void GenHeightMap(double** height_map, const uint32_t map_width,
        const uint32_t map_height, const double init_alt, std::pair<double,uint64_t> rand_seeds)
	{
	  double rva[12];
	  uint64_t ri = rand_seeds.second;

	  for (int i=0; i<12; ++i) {
        rva[i] = DblFromLong(ri) * 0.1;
        rva[i] += rva[i] < 0.0 ? -0.15 : 0.15;
        ri = RandomLong(ri);
	  }

	  /* initialize vertices to slightly irregular tetrahedron */
	  tetra[0].x = -sqrt(3.0)+rva[0];//-0.20;
	  tetra[0].y = -sqrt(3.0)+rva[1];//-0.22;
	  tetra[0].z = -sqrt(3.0)+rva[2];//-0.23;

	  tetra[1].x = -sqrt(3.0)+rva[3];//-0.19;
	  tetra[1].y = sqrt(3.0)+rva[4];//+0.18;
	  tetra[1].z = sqrt(3.0)+rva[5];//+0.17;

	  tetra[2].x = sqrt(3.0)+rva[6];//+0.21;
	  tetra[2].y = -sqrt(3.0)+rva[7];//-0.24;
	  tetra[2].z = sqrt(3.0)+rva[8];//+0.15;

	  tetra[3].x = sqrt(3.0)+rva[9];//+0.24;
	  tetra[3].y = sqrt(3.0)+rva[10];//+0.22;
	  tetra[3].z = -sqrt(3.0)+rva[11];//-0.25;

	  scale = 1.0;

	  rseed = rand_seeds.first;
	  Width = map_width;
	  Height = map_height;
	  heights = height_map;
	  M = init_alt;

	  Depth = 3*((int)(log_2(scale*Height)))+6;

	  r1 = rseed;

	  r1 = rand2(r1,r1);
	  r2 = rand2(r1,r1);
	  r3 = rand2(r1,r2);
	  r4 = rand2(r2,r3);

	  tetra[0].s = r1;
	  tetra[1].s = r2;
	  tetra[2].s = r3;
	  tetra[3].s = r4;

	  tetra[0].h = M;
	  tetra[1].h = M;
	  tetra[2].h = M;
	  tetra[3].h = M;

	  //std::this_thread::sleep_for(gsleep);

	  squarep();
	}

	float DrawColors(RGB32* texture, const cl_Planet& planet) const
	{
	    RGB32 landColor(DIRT_RGB, 255);
	    RGB32 dwaterColor(DWATER_RGB, 255);
	    RGB32 waterColor(WATER_RGB, 255);
	    RGB32 sandColor(SAND_RGB, 255);
	    RGB32 rockColor(ROCK_RGB, 255);
	    RGB32 snowColor(ICE_RGB, 255);
	    RGB32 color;

	    double lowLevel = -0.1;
	    double sandLevel = -0.01;
	    double seaLevel = 0.0;
	    double landLevel = 0.005;
	    double rockLevel = 0.05;
	    double snowLevel = 0.08;
	    //double highLevel = 0.1;

        uint32_t waterPixs=0;

        switch (planet.type) {
            case 0:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 1:
                landColor = AlphaBlend(RGB32(GAS_RGB, 255), RGB32(DESERT_RGB, planet.habitability)); break;
            case 2:
                landColor = AlphaBlend(RGB32(GAS_RGB, 255), RGB32(DESERT_RGB, planet.habitability)); break;
            case 3:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability));
                snowColor.alpha = std::round(200.0f*planet.water_frac*(1.0f-std::min(1.0f,planet.temp/FREEZING_TEMP)));
                landColor = AlphaBlend(landColor, snowColor);
                break;
            case 4:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(DIRT_RGB, planet.habitability));
                snowColor.alpha = std::round(200.0f*planet.water_frac*(1.0f-std::min(1.0f,planet.temp/FREEZING_TEMP)));
                landColor = AlphaBlend(landColor, snowColor);
                break;
            case 5:
                landColor = AlphaBlend(RGB32(ROCK_RGB, 255), RGB32(EARTH_RGB, planet.habitability)); break;
            case 6:
                landColor = AlphaBlend(RGB32(DIRT_RGB, 255), RGB32(EARTH_RGB, planet.habitability)); break;
            case 7:
                landColor = AlphaBlend(RGB32(DESERT_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 8:
                landColor = AlphaBlend(RGB32(SILIC_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 9:
                landColor = AlphaBlend(RGB32(LAVA_RGB, 255), RGB32(DIRT_RGB, planet.habitability));
                waterColor = AlphaBlend(landColor, RGB32(LAVA_RGB, std::round(std::min(255.0f,126.0f*(planet.temp/LAVA_MIN_TEMP)))));
                dwaterColor = AlphaBlend(waterColor, RGB32(LAVA_RGB, std::round(std::min(255.0f,126.0f*(planet.temp/LAVA_MIN_TEMP)))));
                break;
            case 10:
                landColor = AlphaBlend(RGB32(CARBON_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
            case 11:
                landColor = AlphaBlend(RGB32(IRON_RGB, 255), RGB32(DIRT_RGB, planet.habitability)); break;
        }

        landColor.alpha = 50;
        dwaterColor = AlphaBlend(dwaterColor, landColor);
        waterColor = AlphaBlend(waterColor, landColor);
        sandColor = AlphaBlend(sandColor, landColor);
        rockColor = AlphaBlend(rockColor, landColor);
        snowColor = AlphaBlend(snowColor, landColor);

	    for (int x=0; x<Width; ++x)
        {
            for (int y=0; y<Height; ++y)
            {
                double height = heights[x][y];
                if (height < sandLevel) {
                    waterColor.alpha = std::round(255.0 * smoothstep(lowLevel, sandLevel, height));
                    color = AlphaBlend(dwaterColor, waterColor);
                    waterPixs++;
                } else if (height < seaLevel) {
                    sandColor.alpha = std::round(255.0 * smoothstep(sandLevel, seaLevel, height));
                    color = AlphaBlend(waterColor, sandColor);
                    waterPixs++;
                } else if (height < landLevel) {
                    landColor.alpha = std::round(255.0 * smoothstep(seaLevel, landLevel, height));
                    color = AlphaBlend(sandColor, landColor);
                } else if (height < rockLevel) {
                    rockColor.alpha = std::round(255.0 * smoothstep(landLevel, rockLevel, height));
                    color = AlphaBlend(landColor, rockColor);
                } else if (height < snowLevel) {
                    snowColor.alpha = std::round(255.0 * smoothstep(rockLevel, snowLevel, height));
                    color = AlphaBlend(rockColor, snowColor);
                } else {
                    snowColor.alpha = 255;
                    color = snowColor;
                }
                texture[(y * Width) + x] = color;
                //std::this_thread::sleep_for(csleep);
            }
	    }

	    float waterFrac = (float)waterPixs / (Width*Height);
	    return waterFrac + ((planet.water_frac-waterFrac)*0.1);
	}

private:

    double planet(vertex a, vertex b, vertex c, vertex d, double x, double y, double z, int level)
	{
	  vertex e;
	  double lab, lac, lad, lbc, lbd, lcd, maxlength;
	  double es1, es2, es3;
	  double eax,eay,eaz, epx,epy,epz;
	  double ecx,ecy,ecz, edx,edy,edz;
	  //double x1,y1,z1,x2,y2,z2,l1,tmp;

	  if (level>0) {

		/* make sure ab is longest edge */
		lab = dist2(a,b);
		lac = dist2(a,c);
		lad = dist2(a,d);
		lbc = dist2(b,c);
		lbd = dist2(b,d);
		lcd = dist2(c,d);

		maxlength = lab;
		if (lac > maxlength) maxlength = lac;
		if (lad > maxlength) maxlength = lad;
		if (lbc > maxlength) maxlength = lbc;
		if (lbd > maxlength) maxlength = lbd;
		if (lcd > maxlength) maxlength = lcd;

		if (lac == maxlength) return(planet(a,c,b,d, x,y,z, level));
		if (lad == maxlength) return(planet(a,d,b,c, x,y,z, level));
		if (lbc == maxlength) return(planet(b,c,a,d, x,y,z, level));
		if (lbd == maxlength) return(planet(b,d,a,c, x,y,z, level));
		if (lcd == maxlength) return(planet(c,d,a,b, x,y,z, level));

		if (level == 11) { /* save tetrahedron for caching */
		  ssa = a; ssb = b; ssc = c; ssd = d;
		}

		/* ab is longest, so cut ab */
		  e.s = rand2(a.s,b.s);
		  es1 = rand2(e.s,e.s);
		  es2 = 0.5+0.1*rand2(es1,es1);  /* find cut point */
		  es3 = 1.0-es2;

		  if (a.s<b.s) {
			e.x = es2*a.x+es3*b.x; e.y = es2*a.y+es3*b.y; e.z = es2*a.z+es3*b.z;
		  } else if (a.s>b.s) {
			e.x = es3*a.x+es2*b.x; e.y = es3*a.y+es2*b.y; e.z = es3*a.z+es2*b.z;
		  } else { /* as==bs, very unlikely to ever happen */
			e.x = 0.5*a.x+0.5*b.x; e.y = 0.5*a.y+0.5*b.y; e.z = 0.5*a.z+0.5*b.z;
		  }

          if (lab>1.0) lab = pow(lab,0.5);
          /* decrease contribution for very long distances */
          e.h = 0.5*(a.h+b.h) /* average of end points */
            + e.s*dd1*pow(fabs(a.h-b.h),POWA)
            /* plus contribution for altitude diff */
            + es1*dd2*pow(lab,POW); /* plus contribution for distance */


		  /* find out in which new tetrahedron target point is */
		  eax = a.x-e.x; eay = a.y-e.y; eaz = a.z-e.z;
		  ecx = c.x-e.x; ecy = c.y-e.y; ecz = c.z-e.z;
		  edx = d.x-e.x; edy = d.y-e.y; edz = d.z-e.z;
		  epx =   x-e.x; epy =   y-e.y; epz =   z-e.z;
		  if ((eax*ecy*edz+eay*ecz*edx+eaz*ecx*edy
			   -eaz*ecy*edx-eay*ecx*edz-eax*ecz*edy)*
			  (epx*ecy*edz+epy*ecz*edx+epz*ecx*edy
			   -epz*ecy*edx-epy*ecx*edz-epx*ecz*edy)>0.0) {
			/* point is inside acde */
			return(planet(c,d,a,e, x,y,z, level-1));
		  } else {
			/* point is inside bcde */
			return(planet(c,d,b,e, x,y,z, level-1));
		  }
	  }
	  else { /* level == 0 */
		return 0.25*(a.h+b.h+c.h+d.h);
	  }
	}

    double planet1(double x, double y, double z)
	{
	  //vertex a,b,c,d;
	  double abx,aby,abz, acx,acy,acz, adx,ady,adz, apx,apy,apz;
	  double bax,bay,baz, bcx,bcy,bcz, bdx,bdy,bdz, bpx,bpy,bpz;

	  /* check if point is inside cached tetrahedron */

	  abx = ssb.x-ssa.x; aby = ssb.y-ssa.y; abz = ssb.z-ssa.z;
	  acx = ssc.x-ssa.x; acy = ssc.y-ssa.y; acz = ssc.z-ssa.z;
	  adx = ssd.x-ssa.x; ady = ssd.y-ssa.y; adz = ssd.z-ssa.z;
	  apx = x-ssa.x; apy = y-ssa.y; apz = z-ssa.z;

	  if ((adx*aby*acz+ady*abz*acx+adz*abx*acy
		   -adz*aby*acx-ady*abx*acz-adx*abz*acy)*
		  (apx*aby*acz+apy*abz*acx+apz*abx*acy
		   -apz*aby*acx-apy*abx*acz-apx*abz*acy) > 0.0) {
		/* p is on same side of abc as d */
		if ((acx*aby*adz+acy*abz*adx+acz*abx*ady
			 -acz*aby*adx-acy*abx*adz-acx*abz*ady)*
			(apx*aby*adz+apy*abz*adx+apz*abx*ady
			 -apz*aby*adx-apy*abx*adz-apx*abz*ady) > 0.0) {
		  /* p is on same side of abd as c */
		  if ((abx*ady*acz+aby*adz*acx+abz*adx*acy
			   -abz*ady*acx-aby*adx*acz-abx*adz*acy)*
			  (apx*ady*acz+apy*adz*acx+apz*adx*acy
			   -apz*ady*acx-apy*adx*acz-apx*adz*acy) > 0.0) {
			/* p is on same side of acd as b */
			bax = -abx; bay = -aby; baz = -abz;
			bcx = ssc.x-ssb.x; bcy = ssc.y-ssb.y; bcz = ssc.z-ssb.z;
			bdx = ssd.x-ssb.x; bdy = ssd.y-ssb.y; bdz = ssd.z-ssb.z;
			bpx = x-ssb.x; bpy = y-ssb.y; bpz = z-ssb.z;
			if ((bax*bcy*bdz+bay*bcz*bdx+baz*bcx*bdy
				 -baz*bcy*bdx-bay*bcx*bdz-bax*bcz*bdy)*
				(bpx*bcy*bdz+bpy*bcz*bdx+bpz*bcx*bdy
				 -bpz*bcy*bdx-bpy*bcx*bdz-bpx*bcz*bdy) > 0.0) {
			  /* p is on same side of bcd as a */
			  /* Hence, p is inside cached tetrahedron */
			  /* so we start from there */
			  return(planet(ssa,ssb,ssc,ssd, x,y,z, 11));
			}
		  }
		}
	  }
	 /* otherwise, we start from scratch */

	  return(planet(tetra[0], tetra[1], tetra[2], tetra[3],
							 /* vertices of tetrahedron */
					x,y,z,   /* coordinates of point we want colour of */
					Depth)); /* subdivision depth */

	}

    void planet0(double x, double y, double z, int i, int j)
	{
	  double alt, y2;
	  //int colour;

	  alt = planet1(x,y,z);

	  /* store height for heightfield */
	  heights[i][j] = alt;

	  y2 = y*y; y2 = y2*y2; y2 = y2*y2;

	  /* calculate colour */
	  /*if (alt <=0.) { // if below sea level then
		  colour = SEA+(int)((SEA-LOWEST+1)*(10*alt));
		  if (colour<LOWEST) colour = LOWEST;
	  }
	  else {
		if (alt >= 0.1)
		  colour = HIGHEST;
		else {
		  colour = LAND+(int)((HIGHEST-LAND+1)*(10*alt));
		  if (colour>HIGHEST) colour = HIGHEST;
		}
	  }

	  // store colour
	  col[i][j] = colour;*/
	}

	/*void mercator()
	{
	  double y,scale1,cos2,theta1;
	  int i,j,k;

	  y = 1.0;
	  y = 0.5*log(y);
	  k = (int)(0.5*y*Width*scale/PI+0.5);
	  for (j = 0; j < Height; j++) {
		if (debug && ((j % (Height/25)) == 0))
		  {fprintf (stderr, "%c", view); fflush(stderr);}
		y = PI*(2.0*(j-k)-Height)/Width/scale;
		y = exp(2.*y);
		y = (y-1.)/(y+1.);
		scale1 = scale*Width/Height/sqrt(1.0-y*y)/PI;
		cos2 = sqrt(1.0-y*y);
		Depth = 3*((int)(log_2(scale1*Height)))+3;
		for (i = 0; i < Width ; i++) {
		  theta1 = -0.5*PI+PI*(2.0*i-Width)/Width/scale;
		  planet0(cos(theta1)*cos2,y,-sin(theta1)*cos2, i,j);
		}
	  }
	}*/

    void squarep()
	{
	  double y,scale1,theta1,cos2;
	  int i,j;

	  for (j = 0; j < Height; j++) {
		y = (2.0*j-Height)/Width/scale*PI;
		if (!(fabs(y+y)>PI)) {
		  //for (i = 0; i < Width ; i++) {
			//col[i][j] = BACK;
		//} else {
		  cos2 = cos(y);
		  if (cos2>0.0) {
			scale1 = scale*Width/Height/cos2/PI;
			Depth = 3*((int)(log_2(scale1*Height)))+3;
			for (i = 0; i < Width ; i++) {
			  theta1 = -0.5*PI+PI*(2.0*i-Width)/Width/scale;
			  planet0(cos(theta1)*cos2,sin(y),-sin(theta1)*cos2, i,j);
			  //std::this_thread::sleep_for(gsleep);
			}
		  }
		}
	  }
	}

};
