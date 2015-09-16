varying vec3 Normal;
varying vec3 EyeDir;

uniform samplerCube cubeMap;

void main(void)
{
    vec3 reflectedDirection = normalize(reflect(EyeDir, normalize(Normal)));
    reflectedDirection.y = -reflectedDirection.y;
    vec4 fragColor = textureCube(cubeMap, reflectedDirection);
    gl_FragColor =  fragColor ;
}
