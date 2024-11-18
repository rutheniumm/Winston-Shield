#version 300 es
precision highp float;


uniform sampler2D tex;

layout(location = 0) out vec4 FragCol;
layout(location = 1) out vec2 uv;         // Second output, a different color or texture buffer


void main() {
	FragCol = texture(tex, uv);
}