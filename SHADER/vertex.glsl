// Vertex Shader
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aOffset; // Posiition Offsets (prob will pass velocity offsets when im not lazy lol) 
out vec2 TexCoords;


uniform mat4 view;
uniform mat4 projection;

void main()
{

    vec3 worldPos = vec3(0.05) * aPos + aOffset;

    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}