varying vec4 position;
// position of the vertex (and fragment) in view space
varying vec3 varyingNormalDirection;
// surface normal vector in view space
varying vec4 texCoords; // the texture coordinates

void main()
{
    position = gl_ModelViewMatrix * gl_Vertex;
    varyingNormalDirection = normalize(gl_NormalMatrix * gl_Normal);

    texCoords = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
