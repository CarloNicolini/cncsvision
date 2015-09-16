varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;
void main()
{
    vec4 t=vec4(1.0);
    // Sampling The Texture And Passing It To The Frame Buffer
    if (texture_coordinate.x > 1 || texture_coordinate.x < 0 )
    {
        t = vec4(1.0,0.0,0.0,1.0);
    }
    if (texture_coordinate.y > 1 || texture_coordinate.y < 0 )
    {
        t += vec4(0.0,1.0,0.0,1.0);
    }
    gl_FragColor = texture2D(my_color_texture, texture_coordinate);
}
