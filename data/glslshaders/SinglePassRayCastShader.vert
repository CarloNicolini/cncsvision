varying vec4 vPosition;
varying mat4 ModelView;
//varying mat4 ModelviewProjection;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vPosition = gl_Vertex;
    ModelView = gl_ModelViewMatrix;
}
