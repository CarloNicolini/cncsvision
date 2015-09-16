varying vec3 N;
varying vec3 E;

void main()
{
        gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
        N = gl_NormalMatrix * gl_Normal;
	E = vec3(gl_ModelViewMatrix * gl_Vertex);
}
