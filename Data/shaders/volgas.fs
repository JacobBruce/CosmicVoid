#version 420 core

out vec4 outColor;

uniform float gasVis;
uniform vec3 gasColor;
uniform vec3 rayOrigin;
uniform vec3 resolution;
uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;

uniform sampler2D frameTex;
uniform sampler2D depthTex;

vec3 hash3f(vec3 p) { 
    float n = sin(dot(p, vec3(7, 157, 113)));    
    return fract(vec3(2097152, 262144, 32768)*n); 
}

float hash1f(float n)
{
	return fract(sin(n) * 43758.5453);
}

float avg(vec3 v)
{
	return (v.x+v.y+v.z) / 3.0;
}

// hash based 3d value noise
float noise(vec3 x)
{
	vec3 p = floor(x);
	vec3 f = fract(x);

	f = f*f*(3.0 - 2.0 * f);
	float n = p.x + p.y*57.0 + 113.0*p.z;
	return mix(mix(
				mix(hash1f(n + 0.0), hash1f(n + 1.0), f.x),
				mix(hash1f(n + 57.0), hash1f(n + 58.0), f.x),
				f.y),
			mix(
				mix(hash1f(n + 113.0), hash1f(n + 114.0), f.x),
				mix(hash1f(n + 170.0), hash1f(n + 171.0), f.x),
				f.y),
			f.z);
}

float PointDensity(vec3 point)
{
    return noise(point) * 0.1;
}

float RayMarch(vec3 origin, vec3 step_ray, int step_limit, float dens_limit, float dens_div)
{
    float result = 0.0;
	float zdist = 0.0;
	float zstep = 1.0 / step_limit;
	bool chit = true;
    vec3 pos = origin;
	
	vec2 uv = gl_FragCoord.xy / resolution.xy;
	float starDepth = texture(depthTex, uv).x;
    
    for (int i=0; i<step_limit; ++i)
    {
        pos = pos + step_ray;
        result += PointDensity(pos);
		zdist += zstep;

		if (zdist > starDepth && chit) {
			dens_limit *= 1.0 - avg(texture(frameTex, uv).xyz);
			chit = false;
		}

        if (result > dens_limit) return 1.0;
    }
    
	result *= dens_div;
    return result;
}

void main()
{
    // Create unit direction ray
    vec2 sc = gl_FragCoord.xy - resolution.xy*.5;
	vec3 rd = normalize((camForward * resolution.z) + (camRight * sc.x) + (camUp * sc.y));
	
    // Randomize the ray direction
    rd = (rd + (hash3f(rd.zyx)*.006 - .003));
    
    // March ray to get gas density
    float density = RayMarch(rayOrigin, rd*.2, 50, 5.0, .2);

    outColor = vec4(gasColor*density, density*gasVis);
}