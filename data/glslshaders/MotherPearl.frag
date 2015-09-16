uniform samplerCube cubeMap;

varying vec3 ViewDirection;
varying vec3 Normal;

const float mother_pearl_brightness = 1.5;

#define MOTHER_PEARL

void main( void )
{
   vec3  fvNormal         = normalize(Normal);
   vec3  fvViewDirection  = normalize(ViewDirection);
   vec3  fvReflection     = normalize(reflect(fvViewDirection, fvNormal)); 

#ifdef MOTHER_PEARL
   float view_dot_normal = max(dot(fvNormal, fvViewDirection), 0.0);
   float view_dot_normal_inverse = 1.0 - view_dot_normal;

   gl_FragColor = textureCube(cubeMap, fvReflection) * view_dot_normal;
   gl_FragColor.r += mother_pearl_brightness * textureCube(cubeMap, fvReflection + vec3(0.1, 0.0, 0.0) * view_dot_normal_inverse) * (1.0 - view_dot_normal);
   gl_FragColor.g += mother_pearl_brightness * textureCube(cubeMap, fvReflection + vec3(0.0, 0.1, 0.0) * view_dot_normal_inverse) * (1.0 - view_dot_normal);
   gl_FragColor.b += mother_pearl_brightness * textureCube(cubeMap, fvReflection + vec3(0.0, 0.0, 0.1) * view_dot_normal_inverse) * (1.0 - view_dot_normal);
   gl_FragColor.a = 1;
#else
   gl_FragColor = textureCube(cubeMap, fvReflection);
#endif
}
