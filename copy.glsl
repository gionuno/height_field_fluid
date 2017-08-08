#version 130

uniform sampler2D S;

in vec3 f_x;
in vec2 f_t;

out vec4 o_c;

void main()
{
    o_c.rgb = texture2D(S,f_t).rgb;
    o_c.a =1.0;
}

