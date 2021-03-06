#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
int size = 4;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -size; x < size; ++x) 
    {
        for (int y = -size; y < size; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }

	float denom = ( size ) * 2;
	float color = clamp( result / ( ( denom ) * ( denom ) ), 0.0, 1.0 );
    FragColor = vec4( vec3( color ), 1.0 );
}  