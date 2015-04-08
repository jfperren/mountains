#version 330 core
out vec3 color;
in vec2 uv;
uniform sampler2D tex;

void main() {
    color = vec3(1.0, 1.0, 1.0);//texture(tex, uv).rgb;
}
