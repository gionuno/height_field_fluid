#version 130
precision highp float;

uniform sampler2D h;
uniform sampler2D u;

uniform float dt;
uniform float dx;
uniform float  g;

uniform float hmax;
uniform float umax;
ivec2 size = textureSize(h,0);

in vec3 f_x;
in vec2 f_t;

out vec4 o_c;

void main()
{
    vec2 sx = vec2(1.0/size.x,0.0);
    vec2 sy = vec2(0.0,1.0/size.y);

    vec2 uc = texture(u,f_t).rg;
    
    vec2 hc = texture(h,f_t).rg;
    vec2 hw = texture(h,f_t-sx).rg;
    vec2 he = texture(h,f_t+sx).rg;
    vec2 hs = texture(h,f_t-sy).rg;
    vec2 hn = texture(h,f_t+sy).rg;

    uc.r = uc.r - dt*g*(he.r+he.g-hc.r-hc.g)/dx;
    uc.g = uc.g - dt*g*(hn.r+hn.g-hc.r-hc.g)/dx;

    if((hc.r <= 1e-8*dx && hc.g > he.g+he.r)||(he.r <= 1e-8*dx && he.g > hc.r + hc.g))
           uc.r = 0.0;
    if((hc.r <= 1e-8*dx && hc.g > hn.g+hn.r)||(hn.r <= 1e-8*dx && hn.g > hc.r + hc.g))
           uc.g = 0.0;
    o_c.rg = max(-vec2(100.,100.),min(vec2(100.,100.),uc));
    o_c.a =1.0;
}

