#version 300 es
precision highp float;

uniform float near;
uniform float far;

float LinearizeDepth(float depth) {
    // Make sure depth is a float, and we don't mix ints with floats
    float zNdc = 2.0 * depth - 1.0;  // This is a float expression.
    
    // Calculate zEye using floating point values, no int to float conversion here
    float zEye = (2.0 * far * near) / ((far + near) - zNdc * (far - near));  
    
    // Normalize the linear depth value between near and far
    float linearDepth = (zEye - near) / (far - near);  
    return linearDepth;
}

void main() {
    // Assign the linearized depth to gl_FragDepth
    gl_FragDepth = LinearizeDepth(gl_FragCoord.z);
}
