varying vec3 normal;
varying vec4 vertex;
uniform float time;
void main()
{
    // This is the way to form a vertex deformation!
    vertex = gl_Vertex;
    normal = (gl_NormalMatrix) * gl_Normal;
    float r = length(vertex)/10.0;
    vertex.x += r + vertex.x * sin(time/20.0*vertex.x)*cos(time/100.0)*exp(-(vertex.x*vertex.z)/10.0);
    vertex.y += r + vertex.y * sin(time/20.0*vertex.y)*sin(time/100.0)*exp(-(vertex.x*vertex.y)/10.0);
    vertex.z += r + vertex.z * cos(time/20.0*vertex.z)*exp(-(vertex.z*vertex.z)/10.0);
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
