const vec3 Xunitvec = vec3(1.0, 0.0, 0.0);
const vec3 Yunitvec = vec3(0.0, -1.0, 0.0);

uniform sampler2D envmap;

varying vec3 Normal;
varying vec3 EyeDir;

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
    gl_FragColor = texture2D(envmap, index);
}
