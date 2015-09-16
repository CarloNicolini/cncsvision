varying vec3 Normal;
varying vec3 EyeDir;

varying vec3 diffuseLight;
varying vec3 specularLight;
varying vec3 LightIntensity;

uniform samplerCube cubeMap;

void main(void)
{
    vec3 reflectedDirection = normalize(reflect(EyeDir, normalize(Normal)));
    reflectedDirection.y = -reflectedDirection.y;
    vec4 fragColor = textureCube(cubeMap, reflectedDirection);

    // compute final color
    gl_FragColor =   vec4(diffuseLight,1.0)*0.1+ vec4( specularLight,1.0);
}


