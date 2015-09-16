#version 330
//precision highp float;
layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

uniform vec3 planePos;
uniform vec3 planeNormal;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	// This is a passthrough shader that receives gl_Position==gl_Vertex with no previous transformations, so the final gl_Position
	// is projectionMatrix*modelViewMatrix*gl_in[i].gl_Position and that is emitted
	int i=0;
	for (i=0; i<gl_in.length();i++)
	{
		gl_Position = projectionMatrix*modelViewMatrix*gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
