#version 330 core 

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexUV; 

out DATA
{
	vec2 Position;	
	vec2 TexCoords;
}fs_out;

void main()
{             
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0);
    fs_out.TexCoords = vertexUV;
    fs_out.Position = vertexPosition;
}
