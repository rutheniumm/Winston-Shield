#version 300 es
precision highp float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelView;
uniform mat4 projection;

out vec3 fragPosition;

void main() {
    fragPosition = inPosition;
    gl_Position = projection * modelView * vec4(inPosition, 1.0);
}
