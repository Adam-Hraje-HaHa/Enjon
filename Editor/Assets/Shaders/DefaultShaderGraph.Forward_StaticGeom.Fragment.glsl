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

// Fragment Main
void main()
{
	// Base Color
	AlbedoOut = vec4( 1.0, 1.0, 1.0, 1.0 );

}
