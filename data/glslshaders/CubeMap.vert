varying vec3 Normal;
uniform vec3 EyeDir;
uniform samplerCube cubeMap;

void main()
{
        gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
        Normal = gl_NormalMatrix * gl_Normal;
        //EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);
}
