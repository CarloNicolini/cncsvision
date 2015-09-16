varying vec4 vertex;
varying vec3 normal;

void main(void)
{
    gl_FragColor = vec4(vec3(vertex)/10.0+normal,1.0);
}
