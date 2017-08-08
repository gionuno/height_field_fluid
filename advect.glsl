#version 130
precision highp float;

uniform sampler2D u;
uniform sampler2D S;

ivec2 size  = textureSize(S,0);

uniform float umax;
uniform float dt;

in vec3 f_x;
in vec2 f_t;

out vec4 o_c;

void main()
{
    vec2 v =  texture2D(u,f_t).rg;
    o_c.rgb = texture2D(S,f_t-dt*v).rgb;
    o_c.a  = 1.0;
}