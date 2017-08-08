#version 130

uniform sampler2D s;
uniform sampler2D h;
uniform sampler2D u;

uniform float dt;
uniform float dx;

ivec2 size = textureSize(h,0);

in vec3 f_x;
in vec2 f_t;

uniform float umax;
uniform float hmax;

out vec4 o_c;

void main()
{
    vec2 sx = vec2(1.0/size.x,0.0);
    vec2 sy = vec2(0.0,1.0/size.y);
    vec2 p_t = f_t;
    
    float hc = texture(h,p_t).r;
    
    float ue = texture(u,p_t+sx).r;
    float uw = texture(u,p_t-sx).r;
    float vn = texture(u,p_t+sy).g;
    float vs = texture(u,p_t-sy).g; 
    
    float hw = 0.0;
    if(uw <= 0.0)
	hw = texture(h,p_t).r;
    else
        hw = texture(h,p_t-sx).r;

    float he = 0.0;
    if(ue <= 0)
        he = texture(h,p_t+sx).r;
    else
        he = texture(h,p_t).r;

    float hn = 0.0;
    if(vn <= 0)
    	hn = texture(h,p_t+sy).r;
    else
        hn = texture(h,p_t).r;
    
    float hs = 0.0;
    if(vs <= 0.0)
        hs = texture(h,p_t).r;
    else
        hs = texture(h,p_t-sy).r;

    o_c.r = max(0,(hc + dt*(texture(s,p_t).r-(he*ue-hw*uw+hn*vn-hs*vs)/dx)));
    o_c.g = texture(h,p_t).g;
    o_c.a = 1.0;
}

