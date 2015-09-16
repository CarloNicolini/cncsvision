varying vec4 projPos;
varying vec4 vertexPos;

void main(void)
{
	vertexPos = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	projPos = gl_Position;
}
