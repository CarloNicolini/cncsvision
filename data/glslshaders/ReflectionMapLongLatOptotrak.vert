uniform mat4 worldMatrix;
uniform mat4 invTransWorldMatrix;
uniform vec4 camPosition;

varying vec3 Normal;
varying vec3 EyeDir;
uniform vec3 eyeDirection;
void main()
{
        gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
        Normal = gl_NormalMatrix * gl_Normal;
        //EyeDir = eyeDirection;
	EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);
}
