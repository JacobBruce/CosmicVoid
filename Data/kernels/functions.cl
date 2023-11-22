// ----------------------------- //
// ------ NOISE FUNCTIONS ------ //
// ----------------------------- //

double RandDblFromLng(ulong seed) {
	return ((seed%2) ? 1.0 : -1.0) * ((double)seed / ULONG_MAX);
}

ulong RandomLong(ulong seed) {
	seed ^= seed >> 12;
	seed ^= seed << 25;
	seed ^= seed >> 27;
	return seed * 0x2545F4914F6CDD1D;
}

float ffract(float x)
{
	return x - floor(x);
}

double dfract(double x)
{
	return x - floor(x);
}

float3 ffract3(float3 v)
{
	return (float3)(v.x - floor(v.x), v.y - floor(v.y), v.z - floor(v.z));
}

double3 dfract3(double3 v)
{
	return (double3)(v.x - floor(v.x), v.y - floor(v.y), v.z - floor(v.z));
}

float noise(double4 seed) {
    return ffract(dot(sin(dot(seed,(double4)(113.095,78.237,94.429,59.743))),
		dot(seed,(double4)(725.783,557.435,623.189,487.237))));
}

float hashV3(float3 p3)
{
	p3 = ffract3(p3 * (float3)(743.897f,537.163f,695.505f));
	p3 += dot(p3, p3.yzx + (float3)(139.235f,99.457f,159.793f));
	return ffract((p3.x + p3.y) * p3.z);
}

double hashD3(double3 p3)
{
	p3 = dfract3(p3 * (double3)(743.897,537.163,695.505));
	p3 += dot(p3, p3.yzx + (double3)(139.235,99.457,159.793));
	return ffract((p3.x + p3.y) * p3.z);
}

float3 noiseV3(float3 p3) {
	p3 = ffract3(p3 * (float3)(743.897f,537.163f,695.505f));
	p3 += dot(p3, p3.yzx + (float3)(139.235f,99.457f,159.793f));
	return ffract3(p3);
}

double3 noiseD3(double3 p3)
{
	p3 = dfract3(p3 * (double3)(743.897,537.163,695.505));
	p3 += dot(p3, p3.yzx + (double3)(139.235,99.457,159.793));
	return dfract3(p3);
}

// -------------------------------------------------- //

float Grad3(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float Grad2(int hash, float x, float y)
{
    return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}

float Fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

float Noise2(float x, float y)
{
    int X = ((int)floor(x)) & 0xff;
    int Y = ((int)floor(y)) & 0xff;
    x -= floor(x);
    y -= floor(y);
    float u = Fade(x);
    float v = Fade(y);
    int A = (perm[X  ] + Y) & 0xff;
    int B = (perm[X+1] + Y) & 0xff;
    return Lerp(v, Lerp(u, Grad2(perm[A  ], x, y  ), Grad2(perm[B  ], x-1.0f, y  )),
                   Lerp(u, Grad2(perm[A+1], x, y-1.0f), Grad2(perm[B+1], x-1.0f, y-1.0f)));
}

float Noise3(float x, float y, float z)
{
    int X = ((int)floor(x)) & 0xff;
    int Y = ((int)floor(y)) & 0xff;
    int Z = ((int)floor(z)) & 0xff;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);
    int A  = (perm[X  ] + Y) & 0xff;
    int B  = (perm[X+1] + Y) & 0xff;
    int AA = (perm[A  ] + Z) & 0xff;
    int BA = (perm[B  ] + Z) & 0xff;
    int AB = (perm[A+1] + Z) & 0xff;
    int BB = (perm[B+1] + Z) & 0xff;
    return Lerp(w, Lerp(v, Lerp(u, Grad3(perm[AA  ], x, y  , z  ), Grad3(perm[BA  ], x-1.0f, y  , z  )),
                           Lerp(u, Grad3(perm[AB  ], x, y-1.0f, z  ), Grad3(perm[BB  ], x-1.0f, y-1.0f, z  ))),
                   Lerp(v, Lerp(u, Grad3(perm[AA+1], x, y  , z-1.0f), Grad3(perm[BA+1], x-1.0f, y  , z-1.0f)),
                           Lerp(u, Grad3(perm[AB+1], x, y-1.0f, z-1.0f), Grad3(perm[BB+1], x-1.0f, y-1.0f, z-1.0f))));
}

float PerlinFbm(float3 coord, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * Noise3(coord.x, coord.y, coord.z);
        coord *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

// -------------------------------------------------- //

float hash(float3 p) 
{
    return ffract(sin(p.x*15.32758341+p.y*39.786792357+p.z*59.4583127+7.5312) * 43758.236237153)-.5;
}

float3 hash3(float3 p)
{
    return (float3)(hash(p),hash(p+1.5f),hash(p+2.5f));
}

float voronoi(float3 p)
{    
    float3 ip = floor(p);
    float3 fp = ffract3(p);
	float3 s = (float3)(.6,.6,.6);
    float2 r = (float2)(2.,2.);
    for (int i=-1; i<=0; i++) 
	for (int j=-1; j<=0; j++) 
	for (int k=-1; k<=0; k++) 
	{
		float3 g = (float3)(i, j, k);
        float3 pp = fp +g +hash3(ip - g)*s;
        float d = dot(pp, pp);

        if (d < r.x)
		{
            r.y = r.x;
            r.x = d;
		}
        else if(d < r.y)
		{
    		r.y = d;
		}
	}    
    return r.x;
}

float craters(float3 p) {
    float v = voronoi(p);
    return sin(sqrt(v)*PI*2.)*exp(-4.*v);
}

float CraterFbm(float3 p, int octave) {
	
	float f=1.2;
   
	float r = 0.0;	
    for(int i = 0; i<octave; i++){	
		r += fabs(craters( p*f ))/f;       
	    f *= 2.7;
	}
	return r/2.;
}

float cFbm(float3 coord, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * fabs(craters(coord));
        coord *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

// -------------------------------------------------- //

float sample3DHashUI32(uint x, uint y, uint z)
{
    const uint enthropy0 = 1200u;
    const uint enthropy1 = 4500u;
    const uint enthropy2 = 6700u;
    const uint enthropy3 = 8900u;

    uint value = 
        z * enthropy3 * enthropy2 +
        y * enthropy2 + x;

	value += enthropy1;
	value *= 445593459u;
	value ^= enthropy0;

    return (float)(value * value * value) * 2.32830644e-10;
}

float WorleyNoise3D(float u, float v, float w)
{
    float fractU = u - floor(u);
    float fractV = v - floor(v);
    float fractW = w - floor(w);

    u = floor(u);
    v = floor(v);
    w = floor(w);

    float minDistance = 3.40282347e+37f;

    for (float z = -1.0f; z < 2.0f; z += 1.0f)
    {
        for (float y = -1.0f; y < 2.0f; y += 1.0f)
        {
            for (float x = -1.0f; x < 2.0f; x += 1.0f)
            {
                float xSample = x + sample3DHashUI32((uint)(u + x), (uint)(v + y), (uint)(w + z));
                float ySample = y + sample3DHashUI32((uint)(u + x), (uint)(v + y), (uint)(w + z));
                float zSample = z + sample3DHashUI32((uint)(u + x), (uint)(v + y), (uint)(w + z));
                        
                float distance = 
                        (fractU - xSample) * (fractU - xSample) +
                        (fractV - ySample) * (fractV - ySample) +
                        (fractW - zSample) * (fractW - zSample);

                minDistance = min(minDistance, distance);
            }
        }
    }

    return minDistance;
}

float WorleyNoise(float3 coord)
{
    return WorleyNoise3D(coord.x, coord.y, coord.z);
}

float WorleyFbm(float3 coord, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * WorleyNoise(coord);
        coord *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

/*float WavesFbm(float3 n, float time, float r_time)
{
	n *= WAVE_SCALE;
    float w1 = WorleyFbm((float3)(n.x+time, n.y+time, n.z+time), 1);
    float w2 = WorleyFbm((float3)(n.x+r_time, n.y+r_time, n.z+r_time), 2);
    return w1 * w2;
}*/

// --------------------------------------------------- //

float3 mod289v3(float3 x) {
	float v1 = 289.f;
	float v2 = 1.f / v1;
    return x - floor(x * v2) * v1;
}

float4 mod289v4(float4 x) {
	float4 v1 = 289.f;
	float4 v2 = 1.f / v1;
    return x - floor(x * v2) * v1;
}

float4 permute(float4 x) {
    return mod289v4(((x*34.f)+1.f)*x);
}

float4 taylorInvSqrt(float4 r) {
    return 1.79284291400159f - 0.85373472095314f * r;
}

float snoise(float3 v)
{ 
    const float2 C = (float2)(0.1666666666666667f, 0.3333333333333333f);
    const float4 D = (float4)(0.f, 0.5f, 1.f, 2.f);

    // First corner
    float3 i  = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.f - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);

    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    float3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289v3(i); 
    float4 p = permute( permute( permute( 
             i.z + (float4)(0.f, i1.z, i2.z, 1.f))
           + i.y + (float4)(0.f, i1.y, i2.y, 1.f)) 
           + i.x + (float4)(0.f, i1.x, i2.x, 1.f));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857f; // 1.0/7.0
    float3  ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.f * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.f * x_);    // mod(j,N)

    float4 x = x_ *ns.x + ns.yyyy;
    float4 y = y_ *ns.x + ns.yyyy;
    float4 h = 1.f - fabs(x) - fabs(y);

    float4 b0 = (float4)(x.xy, y.xy);
    float4 b1 = (float4)(x.zw, y.zw);

    float4 s0 = floor(b0)*2.f + 1.f;
    float4 s1 = floor(b1)*2.f + 1.f;
    float4 sh = -step(h, (float4)(0.f));

    float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw*sh.zzww;

    float3 p0 = (float3)(a0.xy,h.x);
    float3 p1 = (float3)(a0.zw,h.y);
    float3 p2 = (float3)(a1.xy,h.z);
    float3 p3 = (float3)(a1.zw,h.w);

    //Normalise gradients
    float4 norm = taylorInvSqrt((float4)(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    float4 m = max(0.6f - (float4)(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.f);
    m = m * m;
    return 42.f * dot(m*m, (float4)(dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3)));
}

float CloudFbm(float3 coord, int octave)
{
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * snoise(coord);
        coord *= 2.f;
        w *= 0.5f;
    }
    return f;
}

// ------------------------------ //
// ------ VECTOR FUNCTIONS ------ //
// ------------------------------ //

float3 VecMin(const float3 v, float m)
{
	return (float3)(fmin(v.x,m), fmin(v.y,m), fmin(v.z,m));
}
float3 VecRotX(const float3 v, const double angle)
{
	return (float3)(v.x, (v.z*sin(angle)) + (v.y*cos(angle)), (v.z*cos(angle)) - (v.y*sin(angle)));
}
float3 VecRotY(const float3 v, float angle)
{
	return (float3)((v.z*sin(angle)) + (v.x*cos(angle)), v.y, (v.z*cos(angle)) - (v.x*sin(angle)));
}
float3 VecRotZ(const float3 v, const double angle)
{
	return (float3)((v.x*cos(angle)) - (v.y*sin(angle)), (v.x*sin(angle)) + (v.y*cos(angle)), v.z);
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
double3 OrbitRot(const double rad, const double angle)
{
	return (double3)(cos(angle), 0.0, -sin(angle)) * rad;
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
	unsigned char invAlpha = (unsigned char)255 - c1.alpha;
	result.red = (c2.red * invAlpha + c1.red * c1.alpha) >> 8;
	result.green = (c2.green * invAlpha + c1.green * c1.alpha) >> 8;
	result.blue = (c2.blue * invAlpha + c1.blue * c1.alpha) >> 8;
	result.alpha = 255;
	return result;
}

RGB32 AlphaBlendStar(const RGB32 c1, const uchar w)
{
	RGB32 result;
	unsigned char invAlpha = (unsigned char)255 - c1.alpha;
	result.red = (w * c1.alpha + c1.red * invAlpha) >> 8;
	result.green = (w * c1.alpha + c1.green * invAlpha) >> 8;
	result.blue = (w * c1.alpha + c1.blue * invAlpha) >> 8;
	return result;
}

float3 ColorToVec3(const RGB32 c)
{
	return (float3)(c.red/255.0f, c.green/255.0f, c.blue/255.0f);
}

float4 ColorToVec4(const RGB32 c)
{
	return (float4)(c.red/255.0f, c.green/255.0f, c.blue/255.0f, c.alpha/255.0f);
}

RGB32 Vec3ToColor(float3 v)
{
	RGB32 result;
	result.rgba = (uchar4)((uchar)round(v.x*255.0f), 
				  (uchar)round(v.y*255.0f), (uchar)round(v.z*255.0f), 255);
	return result;
}

RGB32 Vec4ToColor(float4 v)
{
	RGB32 result;
	result.rgba = (uchar4)((uchar)round(v.x*255.0f), 
				  (uchar)round(v.y*255.0f), 
				  (uchar)round(v.z*255.0f),
				  (uchar)round(v.w*255.0f));
	return result;
}

RGB32 ScaleColor(RGB32 color, float g_frac)
{
	color.red = round(fmin(color.red * g_frac, 255.0f));
	color.green = round(fmin(color.green * g_frac, 255.0f));
	color.blue = round(fmin(color.blue * g_frac, 255.0f));
	color.alpha = round(fmin(color.alpha * g_frac, 255.0f));
	return color;
}

RGB32 ColorMultiplyV3(uchar4 c, float3 v) 
{
	RGB32 result;
	result.rgba = (uchar4)((uchar)min(c.x*v.x, 255.0f), 
				  (uchar)min(c.y*v.y, 255.0f),
				  (uchar)min(c.z*v.z, 255.0f), c.w);
	return result;
}

RGB32 ColorMultiplyV4(uchar4 c, float4 v) 
{
	RGB32 result;
	result.rgba = (uchar4)((uchar)min(c.x*v.x, 255.0f), 
				  (uchar)min(c.y*v.y, 255.0f),
				  (uchar)min(c.z*v.z, 255.0f),
				  (uchar)min(c.w*v.w, 255.0f));
	return result;
}

// ------------------------------ //
// ------- MISC FUNCTIONS ------- //
// ------------------------------ //

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

MoonData GetMoonData(uchar moon_type, float planet_frac, ulong rnd1, ulong rnd2)
{
	MoonData result;
	
	switch (moon_type) {
	case 0: 
		result.water = 0.0001f + (0.00001f*(rnd1%9));
		result.score = 5  + (rnd2%3);
		result.color.rgba = atmosphereColors[FIRE_ATMOS];
		break;
	case 1:
		result.water = ((0.4f + (0.05f*(rnd1%9))) + planet_frac) * 0.5f;
		result.score = 30 + (rnd2%3);
		result.color.rgba = atmosphereColors[DEF_ATMOS];
		break;
	case 2:
		result.water = ((0.8f + (0.01f*(rnd1%9))) + planet_frac) * 0.5f;
		result.score = 64 + (rnd2%3);
		result.color.rgba = atmosphereColors[DEF_ATMOS];
		break;
	case 3:
		result.water = ((0.01f + (0.01f*(rnd1%9))) + planet_frac) * 0.5f;
		result.score = 20 + (rnd2%3);
		result.color.rgba = atmosphereColors[DUST_ATMOS];
		break;
	default:
		result.water = 0.0f;
		result.score = 0;
		result.color.rgba = atmosphereColors[DEF_ATMOS];
	}

	return result;
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

// --------------------------------- //
// ------- SHADING FUNCTIONS ------- //
// --------------------------------- //

float3 SphereNormFromUV(float2 uv)
{
	float UVX_2PI = uv.x * PI_MUL_2;
	float SIN_UVY_PI = -sin(uv.y * PI);

	return (float3)(
		cos(UVX_2PI) * SIN_UVY_PI, 
		cos(uv.y * PI), 
		sin(UVX_2PI) * SIN_UVY_PI
	);
}

float2 CalcTerrainMapUV(double3 p, float rot)
{
	return (float2)(ffract(fabs((0.5 + atan2(p.z, p.x) / PI_MUL_2) + rot)), 0.5 - asin(p.y) / PI);
}

double3 CalcWaterNormal(float2 uv, float shift, float strength, float rot)
{
	int pix_X = uv.x * PLANET_TEX_W;
	int pix_Y = uv.y * PLANET_TEX_H;
	
	float halfHeight = PLANET_TEX_H * 0.5f;
	float yDist = (halfHeight - (pix_Y+0.5f)) / halfHeight;
	int xStep = round(1.f / sqrt(1.f - (yDist*yDist)));
	
	uint LX = ((pix_X - xStep) < 0) ? PLANET_TEX_W - xStep : pix_X - xStep;
	uint RX = (uint)(pix_X + xStep) % PLANET_TEX_W;
	uint BY = ((pix_Y - 1) < 0) ? PLANET_TEX_H - 1 : pix_Y - 1;
	uint TY = (pix_Y + 1) % PLANET_TEX_H;

	float3 N = SphereNormFromUV(uv);
	float3 L = SphereNormFromUV((float2)((LX+0.5f)/PLANET_TEX_W, uv.y));
	float3 R = SphereNormFromUV((float2)((RX+0.5f)/PLANET_TEX_W, uv.y));
	float3 B = SphereNormFromUV((float2)(uv.x, (BY+0.5f)/PLANET_TEX_H));
	float3 T = SphereNormFromUV((float2)(uv.x, (TY+0.5f)/PLANET_TEX_H));
	
	float3 vL = (float3)(L.x*WAVE_SCALE+shift, L.y*WAVE_SCALE+shift, L.z*WAVE_SCALE+shift);
	float3 vR = (float3)(R.x*WAVE_SCALE+shift, R.y*WAVE_SCALE+shift, R.z*WAVE_SCALE+shift);
	float3 vB = (float3)(B.x*WAVE_SCALE+shift, B.y*WAVE_SCALE+shift, B.z*WAVE_SCALE+shift);
	float3 vT = (float3)(T.x*WAVE_SCALE+shift, T.y*WAVE_SCALE+shift, T.z*WAVE_SCALE+shift);
	
	float LR = (WorleyFbm(vL, WAVE_DETAIL) - WorleyFbm(vR, WAVE_DETAIL)) * 200.f * strength;
	float BT = (WorleyFbm(vB, WAVE_DETAIL) - WorleyFbm(vT, WAVE_DETAIL)) * 200.f * strength;
	float3 diff = (float3)(0.f,0.f,0.f);
	
	diff = LR < 0.f ? (L - N) * (float3)(-LR,-LR,-LR) : (R - N) * (float3)(LR,LR,LR);
	diff += BT < 0.f ? (B - N) * (float3)(-BT,-BT,-BT) : (T - N) * (float3)(BT,BT,BT);
	
	N = VecRotY(normalize(N + diff), rot*PI_MUL_2);
	
	return (double3)(N.x, N.y, N.z);
}

double3 CalcPlanetNormal(__global float4* map, float2 uv, double3 surf_norm, float m, float rot)
{
	float3 norm = VecRotY(map[((uint)(uv.y*PLANET_TEX_H)*PLANET_TEX_W) + (uint)(uv.x*PLANET_TEX_W)].xyz, rot*PI_MUL_2);
	return normalize((double3)(surf_norm.x+norm.x*m, surf_norm.y+norm.y*m, surf_norm.z+norm.z*m));
}

double3 CalcMoonNormal(__global float4* map, float2 uv, double3 surf_norm, float m, float rot)
{
	float3 norm = VecRotY(map[((uint)(uv.y*MOON_TEX_H)*MOON_TEX_W) + (uint)(uv.x*MOON_TEX_W)].xyz, rot*PI_MUL_2);
	return normalize((double3)(surf_norm.x+norm.x*m, surf_norm.y+norm.y*m, surf_norm.z+norm.z*m));
}

float CalcCloudCover(double3 surf_norm, float shift, float hide, float rot)
{
	surf_norm = VectRotY(surf_norm, -rot*PI_MUL_2);
	return //(hide > 0.49f) ? 0.f : 
	max(0.f, CloudFbm((float3)(
		(float)surf_norm.x+shift,
		(float)surf_norm.y+shift,
		(float)surf_norm.z+shift), 
	5) - hide) / (1.f-hide);
}

float4 CalcSurfColor(read_only image2d_t texture, float2 uv)
{
	int width = uv.x*get_image_width(texture);
	int height = uv.y*get_image_height(texture);

	return read_imagef(texture, (int2)(width, height));
}

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

float3 CalcSurfLight(double3 point, double3 surf_norm, double3 cam_pos, double3 light_pos, 
					 float3 light_color, float4 diffuse, float4 specular, float light_power)
{
	// get distance between light and planet surface
	double sDist = distance(point, light_pos);

	// get light direction ray relative to surface
	double3 losVect = normalize(light_pos - point);

	// get camera direction ray relative to surface
	double3 cosVect = normalize(cam_pos - point);

	// apply Lambertian diffuse reflection to light intensity
	float dotProd = fmax(dot(losVect, surf_norm), 0.0);
	float3 ltiVect = (light_color * (diffuse.xyz * dotProd)) + diffuse.w;

	// apply Blinn-Phong specular reflection to light intensity
	if (dotProd > 0.0f) {
		double3 hlfVect = normalize((losVect + cosVect) * 0.5);
		dotProd = pow((float)dot(hlfVect, surf_norm), specular.w);
		ltiVect += specular.xyz * dotProd;
	}

	// apply inverse square law to light intensity
	return ltiVect * (float)min(1.0, light_power/pow(sDist, 2.0));
}
