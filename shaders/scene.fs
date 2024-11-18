#version 300 es
precision highp float;
out vec4 FragCol;  // Explicitly specify location 0

in vec3 normal;              // Input normal vector
in vec3 lightDirection;      // Input light direction

uniform vec3 color;          // The color of the object

void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = color * ambientStrength;

    // Normalize the vectors to ensure they are unit vectors
    vec3 lightDir = normalize(-lightDirection);
    vec3 norm = normalize(normal);  // Normalize the normal vector

    // Diffuse lighting
    float diffuseStrength = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = color * diffuseStrength;

    // Final color output
    FragCol = vec4((ambient + diffuse), 1.0);
}
