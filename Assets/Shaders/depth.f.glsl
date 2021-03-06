#version 330 core
in vec2 TexCoords;

layout (location = 0) out vec4 diffuse;  	// Diffuse
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normals;

in DATA
{
	vec4 Position;
	vec2 TexCoords;
	vec4 Color;
} fs_in;

in vec4 FragPos;

uniform sampler2D texture1;

void main()
{             
    diffuse = fs_in.Color * texture(texture1, fs_in.TexCoords);
    position = FragPos;
    normals = fs_in.Color * texture(texture1, fs_in.TexCoords);
}

