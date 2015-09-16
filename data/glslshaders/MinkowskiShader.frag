const vec3 Xunitvec = vec3(1.0, 0.0, 0.0);
const vec3 Yunitvec = vec3(0.0, -1.0, 0.0);

uniform sampler2D envmap;

varying vec3 Normal;
varying vec3 EyeDir;
uniform vec3 beta;


const float exposure_compensation_factor=-3.0;
const float prevalence_red_giants_factor=3.0;
const float brightness_distribution_factor=100.0;

uniform float brightness;

uniform  vec3 vel; /// observer velocity.

vec3 blackbody_radiation(float t){/// implements plank law
        const float t_white=6000.0;
        const float b_r=22135.0;
        const float b_g=26159.0;
        const float b_b=31972.0;
        const float a_r=exp(b_r/6000.0)-1.0;
        const float a_g=exp(b_g/6000.0)-1.0;
        const float a_b=exp(b_b/6000.0)-1.0;
        return vec3(a_r/(exp(b_r/t)-1.0), a_g/(exp(b_g/t)-1.0), a_b/(exp(b_b/t)-1.0));
}

float doppler(vec3 v, vec3 dir){/// doppler shift of frequency
        float dl=length(dir);
        float gamma=1.0/sqrt(1.0-dot(v,v));
        return gamma*(1.0+dot(v,dir)/dl);
}

float rand(vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233, 85.34127))) * 43758.5453);
}

void brightness_temperature(out float brightness_factor, out float temperature){
        float a=rand(gl_Vertex.xyz);
        float b=rand(gl_Vertex.xyz+vec3(1.2423));
        float br=1.0/(a+1.0/brightness_distribution_factor);
        const float tmp_distr=prevalence_red_giants_factor;
        float t=exp(-b*tmp_distr)*50000.0;
        temperature=t;
        vec3 c=blackbody_radiation(t);
        float lc=length(c);
        brightness_factor=br/lc;

}


vec3 aberration(vec3 v, vec3 dir, float solid_angle_factor){/// todo: verify that the solid_angle_factor works correctly.
        float u=length(v);
        float dl=length(dir);
        if(u<1.0E-20)return dir/dl;
        float dp=dot(v,dir);
        float old_c=dp/dl;
        float old_s=sqrt(1.0-old_c*old_c);
        float new_c=(u+dp/(u*dl))/(1.0+dp/dl); /// new_c=(u+old_c)/(1.0+u*old_c)
        // float new_c=dp/(u*dl);/// test: new cosine of angle equal to old cosine
        float new_s=sqrt(1.0-new_c*new_c); /// new sine of angle
        vec3 sv=normalize(dir-v*dp/(u*u)); /// old sine vector
        vec3 new_dir=new_c*v/u + sv*new_s;
        /// two parts: the decrease in sine shrinks the solid angle proportionally to sine, while the derivative of cosine shrinks by its value
        solid_angle_factor=(new_s/old_s) * (1.0-u*u)/pow((u*old_c+1.0),2);


        return new_dir;
}


#define INV_PI		0.318309886

void main()
{
    vec3 reflectDir = reflect(EyeDir, Normal);
    vec2 index;
    
    /* Vertical coordinate */
    float d = dot( normalize(reflectDir), Yunitvec);
    index.y = acos(d) * INV_PI;

    /* Horizontal coordinate */
    reflectDir.y = 0.0;
    d = dot( normalize(reflectDir), Xunitvec);
    index.x = acos(d) * INV_PI;
    if (reflectDir.z < 0.0) {
        index.x = (index.x * 0.5) + 0.5;
    } else {
        index.x = (1.0 - index.x) * 0.5;
    }
    gl_FragColor = vec4(1.0,0.0,0.0,1.0);//texture2D(envmap, index);
}
