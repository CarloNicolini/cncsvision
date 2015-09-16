varying vec3 texture_coordinate;
uniform float c;

void main()
{
    vec4 v=gl_Vertex;
    v.z = -c*(v.x*v.x+v.y*v.y);
    texture_coordinate = vec3(v.x+0.5,v.y+0.5,v.z+0.5);
    gl_Position = gl_ModelViewProjectionMatrix*v;
}
