#version 300 es
precision mediump float;

in vec2 fragTexCoord;
uniform sampler2D ssaoTexture;
out vec4 fragColor;

void main() {
    vec3 ssao = texture(ssaoTexture, fragTexCoord).rgb;
    fragColor = vec4(ssao, 1.0);
}
