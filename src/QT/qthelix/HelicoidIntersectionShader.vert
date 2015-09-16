//uniform float objectScale;
//uniform vec3 objectTranslation;
//uniform mat4 objectRotation;
uniform mat4 objectTransformation;
void main()
{
    vec4 vr = objectTransformation*vec4(gl_Vertex.xyz,1.0);
    gl_Position    =  vr;
}
