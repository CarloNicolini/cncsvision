uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	// This vertex shader sends the vertices as gl_Position to the geometry shader.
	gl_Position    =  gl_Vertex;
}

