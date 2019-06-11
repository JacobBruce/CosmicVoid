#version 420 core
// Modification of shader from:
// https://www.shadertoy.com/view/XdfXRX

in vec2 Texcoord;

out vec4 outColor;

uniform vec3 lightColor;
uniform vec2 lightPos;
uniform float luminosity;
uniform float aspectRatio;
uniform float iTime;
uniform float dFrac;
uniform float tFrac;
uniform float alpha;

float noise(float t)
{
	return fract(sin(t)*57839.371);
}

vec4 modcolor(vec3 color, float factor,float factor2)
{
	float w = color.x+color.y+color.z;
	return vec4(mix(color,vec3(w)*factor,w*factor2), w*alpha*0.4);
}

vec3 lensflare(vec2 uv,vec2 pos)
{
	vec2 main = uv-pos;
	vec2 uvd = uv*(length(uv));
	
	float ang = atan(main.y, main.x);
	float dist=length(main); dist = pow(dist,.1);
	float n = 0.1 + (noise(ang+iTime*dist)*0.6);
	
	float f0 = 1.0/(length(uv-pos)*16.0+1.0);
	
	f0 = f0+f0*(sin((ang+iTime/18.0 + noise(floor(10.0*abs(ang)+n/2.0))*2.0)*12.0)*.1+dist*.1+.8);

	float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
	float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
	float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;
	
	vec2 uvx = mix(uv,uvd,-0.5);
	
	float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
	float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
	
	uvx = mix(uv,uvd,-.4);
	
	float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
	float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
	float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
	
	uvx = mix(uv,uvd,-0.5);
	
	float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
	float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
	
	vec3 c = vec3(.0);
	
	c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
	c+=vec3(f0);
	
	return c;
}

void main()
{
	vec2 uv = (Texcoord-0.5) * luminosity;
	uv.x *= aspectRatio;
	vec3 c = lightColor * lensflare(uv,lightPos*luminosity);
	outColor = modcolor(c, dFrac, tFrac);
}