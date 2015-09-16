varying vec2 texture_coordinate;
uniform float height;
uniform int wireframe;
float pi = 3.141592654;

void main()
{
    // Transforming The Vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    float thetax = cos(atan2(gl_Vertex.z, gl_Vertex.x));
    texture_coordinate.x = 0.5*(thetax+1.0);// genera l'angolo come campionatore della coordinata s della texture
    // Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
    float thetay = atan2(gl_Vertex.y, height);

    //texture_coordinate.y = 0.5*abs(gl_Vertex.y/(height)+1);//0.5*((thetay)/(0.5*pi)+1);
    //texture_coordinate.y = 0.5*(thetay/(0.5*pi)+1);
    texture_coordinate.y = 0.5*(gl_Vertex.y/height+1);
}
