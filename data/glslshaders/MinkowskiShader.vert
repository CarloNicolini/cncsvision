varying vec3 Normal;
varying vec3 EyeDir;

uniform vec3 beta;
uniform float time;

vec3 aberration(vec3 v, vec3 dir, float solid_angle_factor)
{
    /// todo: verify that the solid_angle_factor works correctly.
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

void main()
{
    EyeDir = vec3(gl_ModelViewMatrix*vec4( 5.0,0.0,0.0,0.0)).xyz;

    float b2 = (beta.x*beta.x + beta.y*beta.y + beta.z*beta.z )+1E-12; //to avoid NaN in lorentzTransformation matrix
    float g=1.0/(sqrt(abs(1.0-b2))+1E-12);
    float q=(g-1.0)/b2;

    //http://en.wikipedia.org/wiki/Lorentz_transformation#Matrix_forms
    vec4 tmpVertex = vec4( ((gl_Vertex)).xyz-EyeDir,time) +vec4(0.0,0.0,-5.0,0.0) ;
    float w = 1.0;

    mat4  lorentzTransformation =
            mat4(
                1.0+beta.x*beta.x*q ,   beta.x*beta.y*q ,   beta.x*beta.z*q , -beta.x*g ,
                beta.y*beta.x*q , 1.0+beta.y*beta.y*q ,   beta.y*beta.z*q , -beta.y*g ,
                beta.z*beta.x*q ,   beta.z*beta.y*q , 1.0+beta.z*beta.z*q , -beta.z*g ,
                -beta.x*g , -beta.y*g , -beta.z*g , g
                );

    vec4 vertex2 = vec4(tmpVertex.xyz,time)*(lorentzTransformation);

    gl_Position = gl_ProjectionMatrix*(vec4(vertex2.xyz,1.0) );

}

