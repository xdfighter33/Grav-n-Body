// Vertex Shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aOffsetPosition; // Posiition Offsets (prob will pass velocity offsets when im not lazy lol)
layout (location = 3) in float aMass;     // Mass of body
layout (location = 4) in vec3 aVelocity;  // Velocity for streak effect 
out vec2 TexCoords;


uniform mat4 view;
uniform mat4 projection;

void main()
{

    vec3 worldPos = vec3(aMass) * aPos + aOffsetPosition;
    
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}