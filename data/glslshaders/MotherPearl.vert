uniform vec3 fvEyePosition;
uniform float time;
varying vec3 ViewDirection;
varying vec3 Normal;

void main( void )
{
    vec4 v = gl_Vertex;
    Normal         = gl_NormalMatrix * gl_Normal;
    v.xyz+=sin(time*20.0*length(v.xz))*normalize(Normal)/200.0;
   gl_Position = gl_ModelViewProjectionMatrix*(v);
   vec4 fvObjectPosition = gl_ModelViewMatrix * v;

   ViewDirection  = fvEyePosition - fvObjectPosition.xyz;
}
