varying vec3 normal;
varying vec4 vertex;

uniform float time;
float omega=0.01;
float sigma=1;

void main()
{
       // This is the way to form a vertex deformation!
        vertex = gl_Vertex;
        float x = exp(-length(vertex)/(sigma*sigma))*sin(omega*time);
        float y = exp(-length(vertex)/(sigma*sigma))*cos(omega*time);

        normal = (gl_NormalMatrix) * gl_Normal;
        gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
