varying vec4 texCoord0;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord0 = gl_MultiTexCoord0;
}
