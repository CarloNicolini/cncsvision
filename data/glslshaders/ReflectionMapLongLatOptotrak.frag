uniform sampler2D envmap;
varying vec3 Normal;
varying vec3 EyeDir;

void main()
{
    vec3 r = reflect(normalize(EyeDir),normalize(Normal));
    vec3 reflectedDirection = r/2.0-0.5;
    gl_FragColor = texture2D(envmap, reflectedDirection);
}
