#version 300 es
precision highp float;

in vec3 fragPosition;

void main() {
    // This shader only writes to the depth buffer. No need to output a color value.
    // The depth value is automatically written by the GPU based on the gl_Position in the vertex shader.
    // gl_FragColor is not used here.
}
