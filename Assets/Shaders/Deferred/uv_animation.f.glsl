#version 330 core

layout (location = 0) out vec4 DiffuseOut;     // Diffuse
layout (location = 1) out vec4 NormalsOut;
layout (location = 2) out vec4 PositionOut;
layout (location = 3) out vec4 EmissiveOut;
layout (location = 4) out vec4 MatOut;
layout (location = 5) out vec4 HeightOut;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
    mat3 TBN;
} fs_in;

// uniforms
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;
uniform float maxHeight = 1.0;

void main()
{
    // Translate normal to world space
    vec3 normal = texture(normalMap, (fs_in.TexCoords)).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    float Height = fs_in.FragPos.y / maxHeight;

    // Get diffuse color
    vec4 color = texture(diffuseMap, fs_in.TexCoords);
    if (color.a < 0.5) discard;
    
    DiffuseOut  = color;
    NormalsOut  = vec4(normal, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    EmissiveOut = texture(emissiveMap, fs_in.TexCoords) * vec4(4, 4, 4, 1);
    MatOut      = vec4(0.1, 1.0, 0.0, 1.0);
    HeightOut   = vec4(Height, Height, Height, 1.0);
}