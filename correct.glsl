#version 130

uniform sampler2D P0;
uniform sampler2D P1;

ivec2 size  = textureSize(P0,0);

uniform float umax;
uniform float dt;

in vec3 f_x;
in vec2 f_t;

out vec4 o_c;

void main()
{
    vec2 p0 = texture2D(P0,f_t).rg;
    vec2 p1 = texture2D(P1,f_t).rg;

    o_c.rg = 0.5*(3.0*p0-p1);
    o_c.a  = 1.0;
}