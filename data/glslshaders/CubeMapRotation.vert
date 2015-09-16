varying vec3 Normal;
varying vec3 EyeDir;
uniform samplerCube cubeMap;
uniform float Time;
void main()
{
        gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
        Normal = gl_NormalMatrix * gl_Normal;
        EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);
}

