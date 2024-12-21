#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    // Calculate distance from center
    vec2 center = vec2(0.5, 0.5);
    float dist = length(TexCoords - center) * 2.0;

    // Create smooth circular mask
    float alpha = smoothstep(1.0, 0.8, dist);

    // Sample texture and apply mask
    vec4 texColor = texture(particleTexture, TexCoords);
    FragColor = texColor * vec4(1.0, 1.0, 1.0, alpha);
}