varying vec3 Normal;
varying vec3 EyeDir;
uniform samplerCube cubeMap;
uniform vec3 eyeDirection;

void main()
{
        gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
        Normal = gl_NormalMatrix * gl_Normal;
        EyeDir = eyeDirection;
}
