#version 330 core

layout (location = 0) out vec4 AlbedoOut;
layout (location = 1) out vec4 NormalsOut;
layout (location = 2) out vec4 PositionOut;
layout (location = 3) out vec4 EmissiveOut;
layout (location = 4) out vec4 MatPropsOut;

layout (location = 5) out vec4 ObjectIDOut;


in VS_OUT
{
	vec3 FragPositionWorldSpace;
	vec2 TexCoords;
	mat3 TBN;
	mat3 TS_TBN;
	vec3 ViewPositionTangentSpace;
	vec3 FragPositionTangentSpace;
	vec4 ObjectID;
} fs_in;

// Global Uniforms
uniform float uWorldTime = 1.0f;
uniform vec3 uViewPositionWorldSpace;

// Variable Declarations
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughMap;
uniform float emissiveIntensity;

uniform sampler2D emissiveMap;

vec3 emissiveMult;
uniform sampler2D aoMap;

// Fragment Main
void main()
{
	// Base Color
vec4 albedoMap_sampler = texture( albedoMap, fs_in.TexCoords );
	AlbedoOut = vec4(albedoMap_sampler.rgb, 1.0);

}
