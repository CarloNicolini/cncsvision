varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;
uniform float height;
uniform int wireframe;

void main()
{
    // Sampling The Texture And Passing It To The Frame Buffer
    if (wireframe==1)
        gl_FragColor = vec4(1.0);
    if (wireframe==0)
    {
        //gl_FragColor = vec4(texture_coordinate.x,0,0,1.0);
        gl_FragColor = texture2D(my_color_texture, texture_coordinate );
    }
}
