varying vec4 projPos;
varying vec4 vertexPos;

void main(void)
{
	if (vertexPos.x > 0 )
	gl_FragColor = vec4(1.0,1.0,1.0,1.0);
	else
	gl_FragColor = vec4(sin(vertexPos.z),0.0,0.0,vertexPos.z);
}
