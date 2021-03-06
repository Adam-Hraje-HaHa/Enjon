#version 330 core 

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexUV; 

out DATA
{
	vec2 Position;	
	vec2 TexCoords;
}fs_out;

out vec2 v_blurTexCoords[16];

uniform float u_blurRadius;

void main()
{             
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0);
    fs_out.TexCoords = vertexUV;
    fs_out.Position = vertexPosition;

	vec2 v_texCoord = fs_out.TexCoords;
	v_blurTexCoords[ 0] = v_texCoord + vec2(0.0, -8 * u_blurRadius);
	v_blurTexCoords[ 1] = v_texCoord + vec2(0.0, -7 * u_blurRadius);
	v_blurTexCoords[ 2] = v_texCoord + vec2(0.0, -6 * u_blurRadius);
	v_blurTexCoords[ 3] = v_texCoord + vec2(0.0, -5 * u_blurRadius);
	v_blurTexCoords[ 4] = v_texCoord + vec2(0.0, -4 * u_blurRadius);
	v_blurTexCoords[ 5] = v_texCoord + vec2(0.0, -3 * u_blurRadius);
	v_blurTexCoords[ 6] = v_texCoord + vec2(0.0, -2 * u_blurRadius);
	v_blurTexCoords[ 7] = v_texCoord + vec2(0.0, -1 * u_blurRadius);	// middle
	v_blurTexCoords[ 8] = v_texCoord + vec2(0.0,  1 * u_blurRadius);  // middle
	v_blurTexCoords[ 9] = v_texCoord + vec2(0.0,  2 * u_blurRadius);
	v_blurTexCoords[10] = v_texCoord + vec2(0.0,  3 * u_blurRadius);
	v_blurTexCoords[11] = v_texCoord + vec2(0.0,  4 * u_blurRadius);
	v_blurTexCoords[12] = v_texCoord + vec2(0.0,  5 * u_blurRadius);
	v_blurTexCoords[13] = v_texCoord + vec2(0.0,  6 * u_blurRadius);
	v_blurTexCoords[14] = v_texCoord + vec2(0.0,  7 * u_blurRadius);
	v_blurTexCoords[15] = v_texCoord + vec2(0.0,  8 * u_blurRadius);
}