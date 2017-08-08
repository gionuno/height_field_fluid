#version 130
precision highp float;

uniform sampler2D h;
uniform sampler2D u;

ivec2 size = textureSize(h,0);

uniform float umax;
uniform float hmax;

in vec3 f_x;
in vec2 f_t;

vec3 l_x = vec3(10.0,100.0,0.0);

out vec4 o_c;

vec3 phong_water(float h_,float t,vec2 p,vec3 n)
{
    vec3 x = vec3(p.x,h_ + t,p.y);	
    vec3 d = normalize(l_x - x);
    float ndotl = dot(-n,d);
    float intensity = max(0.0,ndotl);
    vec3 color_aux = intensity*(1-exp(-h_))*vec3(0.0,0.0,1.0);
    vec3 H = normalize(d + vec3(0.0,-1.0,0.0));
    float ndoth = dot(-n,H);
    intensity = pow(max(0.0,ndoth),0.01);
    color_aux += intensity*vec3(0.0,0.9,0.9);
    return color_aux;
}

vec3 phong_terrain(float t,vec2 p,vec3 n)
{
    vec3 x = vec3(p.x,t,p.y);
    vec3 d = normalize(l_x - x);
    float ndotl = dot(-n,d);
    float intensity = max(0.0,ndotl);
    vec3 color_aux = intensity * mix(vec3(0.8,0.7,0.0),vec3(0.0,0.4,0.0),t);
    return color_aux;
}

vec3 phonging(vec2 tex)
{
    vec2 sx = vec2(1.0/size.x,0.0);
    vec2 sy = vec2(0.0,1.0/size.y);
    vec2 H = texture2D(h,tex).rg;
    vec2 H_left = texture2D(h,tex-sx).rg-H;
    vec2 H_down = texture2D(h,tex-sy).rg-H;

    vec3 normal_water = normalize(cross(vec3(1.0,H_left.r+H_left.g,0.0),vec3(0.0,H_down.r+H_down.g,1.0))); 
    vec3 normal_terra = normalize(cross(vec3(1.0,H_left.g,0.0),vec3(0.0,H_down.g,1.0)));

    vec3 color_water = phong_water(H.r,H.g,tex-0.5,normal_water);
    vec3 color_terrain = phong_terrain(H.g,tex-0.5,normal_terra);
    return mix(color_terrain,color_water,1-exp(-H.r));
}

void main()
{
    o_c.rgb = phonging(f_t);
    //o_c.rg  = d*(0.5+0.5*normalize(U));
    o_c.a =1.0;
}

