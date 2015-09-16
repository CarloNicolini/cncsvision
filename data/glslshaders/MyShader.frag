/*
Built-In Types
GLSL has some built-in attributes in a vertex shader:
gl_Vertex				4D vector representing the vertex position
gl_Normal				3D vector representing the vertex normal
gl_Color				4D vector representing the vertex color
gl_MultiTexCoordX			4D vector representing the texture coordinate of texture unit X
GLSL also has some built-in uniforms:
gl_ModelViewMatrix			4x4 Matrix representing the model-view matrix.
gl_ModelViewProjectionMatrix		4x4 Matrix representing the model-view-projection matrix.
gl_NormalMatrix				3x3 Matrix representing the inverse transpose model-view matrix.
                                        This matrix is used for normal transformation.
GLSL Built-In Varyings:
gl_FrontColor				4D vector representing the primitives front color
gl_BackColor				4D vector representing the primitives back color
gl_TexCoord[X]				4D vector representing the Xth texture coordinate

And last but not least there are some built-in types which are used for shader output:
gl_Position				4D vector representing the final processed vertex position. Only
                                        available in vertex shader.
gl_FragColor				4D vector representing the final color which is written in the frame
                                        buffer. Only available in fragment shader.
gl_FragDepth				float representing the depth which is written in the depth buffer.
                                        Only available in fragment shader.
*/
varying vec3 normal;
varying vec3 vertex_to_light_vector;

void main()
{
    // Defining The Material Colors
    const vec4 AmbientColor = vec4(0.1, 0.0, 0.0, 1.0);
    const vec4 DiffuseColor = vec4(1.0, 0.0, 0.0, 1.0);

    // Scaling The Input Vector To Length 1
    vec3 normalized_normal = normalize(normal);
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);

    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

    // Calculating The Final Color
    gl_FragColor = AmbientColor + DiffuseColor * DiffuseTerm;
}
