#version 330 core

in DATA
{
	vec2 Position;	
	vec4 Color;
	vec2 TexCoords;
}fs_in;

out vec4 color;

uniform sampler2D tex;
uniform sampler2D blurTexSmall;
uniform sampler2D blurTexMedium;
uniform sampler2D blurTexLarge;
uniform float exposure;
uniform float gamma;
uniform float bloomScalar;
uniform float Saturation;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;


vec3 Uncharted2Tonemap(vec3 x)
{
     return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() 
{
	// Mix bloom
	vec3 hdrColor = texture2D(tex, fs_in.TexCoords).rgb;
	vec3 bloomColor = texture2D(blurTexSmall, fs_in.TexCoords).rgb + 
						texture2D(blurTexMedium, fs_in.TexCoords).rgb + 
						texture2D(blurTexLarge, fs_in.TexCoords).rgb;
	bloomColor = bloomColor / 3.0;
	hdrColor += bloomColor * bloomScalar;

	// Tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));

	// Saturation
	float lum = result.r * 0.2 + result.g * 0.7 + result.b * 0.1;
	vec3 diff = result.rgb - vec3(lum);

	// Final
	color = vec4(vec3(diff) * Saturation + lum, 1.0);
	// color = vec4(hdrColor, 1.0);
}

