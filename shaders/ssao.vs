#version 300 es
precision mediump float;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelView;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 fragNormal;

void main() {
    fragPosition = vec3(modelView * vec4(position, 1.0));
    fragNormal = normalize(normal);
    gl_Position = projection * vec4(fragPosition, 1.0);
}
