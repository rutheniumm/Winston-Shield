#version 300 es
precision mediump float;

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 sampleKernel[16];  // SSAO kernel
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D noiseTexture;
uniform float radius;
uniform float bias;

out vec4 fragColor;

void main() {
    // Use depth and normal information for SSAO calculation
    vec3 sample = fragPosition;
    vec3 normal = normalize(fragNormal);
    float depth = texture(depthTexture, fragPosition.xy).r;

    // Sample surrounding points based on SSAO kernel
    // Calculate occlusion factor here (you can use random samples, etc.)
    
    fragColor = vec4(vec3(0.0), 1.0);
}
