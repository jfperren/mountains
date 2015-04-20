#version 330 core
out vec4 color;
in vec2 uv;
uniform sampler2D tex_main;
uniform sampler2D tex_mirror;
uniform sampler2D tex_height;

void main() {
    
    // Get the size of the screen (same as size of tex_mirror)
    ivec2 texture_size = textureSize(tex_mirror, 0);
    
    // FragCoord gives pixel coordinates, transform it into uv coordinates by normalizing
    float _u = gl_FragCoord[0] / texture_size[0];
    float _v = 1-gl_FragCoord[1] / texture_size[1];
    
    // Get texture color w/ usual uv, mirror with computed _u/_v
    vec3 texture_color = vec3(0.1, 0.5, 0.9);
    vec3 mirror_color = texture(tex_mirror, vec2(_u, _v)).rgb;
    
    // Mix the two together
    color = vec4(mix(texture_color, mirror_color, vec3(.25)), 0.9);
}