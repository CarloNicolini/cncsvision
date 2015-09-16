varying vec2 texture_coordinate;
/*
void main()
{
    // Transforming The Vertex
    vec4 v = gl_Vertex;
    v.z=-(v.y*v.y+v.x*v.x);
    float rho = sqrt(v.x*v.x+v.z*v.z);
    float theta=atan(v.z,v.x);

    gl_Position = gl_ModelViewProjectionMatrix * v;

    // Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
    float f = rho*(4.0*rho*rho+1.0);
    vec2 tex = vec2(v.x/2+0.5, v.z/2+0.5); // versione base
    float s = v.x*v.y;
    float t = 0.5*(v.x*v.x-v.z*v.z);
    //texture_coordinate =  (vR.xy / (2.0*(1.0 + vR.z))) + 0.5;
    float oow = 1.0/gl_Position.w;
    vec3 pos = (gl_Position.xyz/gl_Position.w)*0.5+0.5;
    //gl_Position.xyz *= oow;

    texture_coordinate = vec2(pos.x,pos.y);
}
*/
/*
mat3 GetLinearPart( mat4 m )
{
    mat3 result;
    result[0][0] = m[0][0];
    result[0][1] = m[0][1];
    result[0][2] = m[0][2];

    result[1][0] = m[1][0];
    result[1][1] = m[1][1];
    result[1][2] = m[1][2];

    result[2][0] = m[2][0];
    result[2][1] = m[2][1];
    result[2][2] = m[2][2];

    return result;
}
*/
float pi = 3.14159265359;
float c=2.0;
float maxRho = 0.707;
void main()
{
    vec4 v=gl_Vertex;
    v.z = -c*(v.x*v.x+v.y*v.y);
    /*
    Conformal
    Let's start with the conformality requirement, and let's also preserve the rotational symmetry and stick to polar coordinates.
    If your map was conformal, then the scale factor between texture coordinates and object coordinates would be independent of direction.
    */

    float rho = sqrt(v.x*v.x+v.y*v.y)/maxRho;
    float theta = atan(v.y,v.x);
    float s= sqrt(4.0*c*c*rho*rho+1.0);
    float k1=0.2;
    float r =k1*(rho/(1.0+s))*exp(s);

    texture_coordinate = (vec2((r*cos(theta)), (r*sin(theta)))+vec2(1))*vec2(0.5);
    gl_Position = gl_ModelViewProjectionMatrix*v;

    /*
    Constant area element
    So now about this constant area element. This would be a parametrization such that equal changes in r and θ correspond to equal area on the paraboloid, no matter the current position.
    */
/*
    float rho = sqrt(v.x*v.x+v.y*v.y);
    float theta = atan(v.y,v.x);
    float s= sqrt(4*rho*rho+1);
    float a=1.0;
    float r = (pow(4*rho*rho+1,1.5) -1)/(12*a) ;
    texture_coordinate = (vec2((r*cos(theta)), (r*sin(theta)))+vec2(0.5));
    gl_Position = gl_ModelViewProjectionMatrix*v;
*/
}
