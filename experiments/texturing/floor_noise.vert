varying vec4 vertex;
uniform vec3 cubeSize;
void main()
{
    vertex = gl_Vertex;
    vec4 v = gl_Vertex;
    v.xyz *= cubeSize;
    gl_Position = gl_Position = gl_ModelViewProjectionMatrix*v;
}
