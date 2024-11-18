#version 300 es
precision mediump float;

layout(location = 0) in vec2 inTexCoord;
out vec2 fragTexCoord;

void main() {
    fragTexCoord = inTexCoord;
    gl_Position = vec4(inTexCoord, 0.0, 1.0);
}
