varying vec3 Normal;
varying vec3 EyeDir;

uniform samplerCube cubeMap;
uniform float Time;

void rotateAngleAxis(in vec3 x, in vec3 axis, in float angle, out vec3 y )
{
    float c=cos(angle);
    float s=sin(angle);
    float t=1.0-c;
    vec3 normaxis = normalize(axis);
    float xi = normaxis.x;
    float yi = normaxis.y;
    float zi = normaxis.z;

    mat3 R = mat3(
                t*xi*xi+c,  t*xi*yi +s*zi  ,t*xi*zi-s*yi ,
                t*xi*yi-s*zi,   t*yi*yi+c,  t*yi*zi+s*xi ,
                t*xi*zi+s*yi,   t*yi*zi-s*xi,   t*zi*zi+c
            );
    y = R*x;
}

void main(void)
{
    vec3 reflectedDirection = normalize(reflect(EyeDir, normalize(Normal)));

    rotateAngleAxis(reflectedDirection, vec3(1.0,1.0,0.0), Time,reflectedDirection);

    reflectedDirection.y = -reflectedDirection.y;
    vec4 fragColor = textureCube(cubeMap, reflectedDirection);
    gl_FragColor = fragColor;
}

