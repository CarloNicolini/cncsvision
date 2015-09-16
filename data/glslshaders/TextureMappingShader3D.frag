varying vec3 texture_coordinate;
uniform sampler3D my_color_texture;
void main()
{
    gl_FragColor =  texture3D(my_color_texture, texture_coordinate);
}
