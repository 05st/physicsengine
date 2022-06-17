#version 460 core
in vec2 texCoord_;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
    FragColor = texture(texture_diffuse1, texCoord_);
    // FragColor = vec4(245.0/255.0, 246.0/255.0, 250.0/255.0, 1.0);
}