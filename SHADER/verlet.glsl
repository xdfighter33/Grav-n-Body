#version 430 core

// Define the structure to match your Particle class
struct Particle {
    vec3 position;
    vec3 velocity;
    vec3 accel;
    vec3 force;
    float mass;
};

// Input/output buffer of particles
layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

// Uniform variables
layout(location = 0) uniform float dt;
layout(location = 1) uniform float g; // Gravitational constant

// Work group size - adjust based on your GPU capabilities
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
    uint gid = gl_GlobalInvocationID.x;
    
    // Ensure we don't go out of bounds
    if(gid >= particles.length())
        return;
    
    Particle p = particles[gid];
    
    // Step 1: Update position
    p.position += p.velocity * dt + 0.5 * p.accel * dt * dt;
    
    // Store old acceleration
    vec3 old_accel = p.accel;
    
    // Step 2: Compute new acceleration
    p.accel = p.force / p.mass;
    
    // Step 3: Compute new velocity
    p.velocity += 0.5 * (old_accel + p.accel) * dt;
    
    // Reset force for the next frame
    p.force = vec3(0.0);
    
    // Write updated particle back to buffer
    particles[gid] = p;
}