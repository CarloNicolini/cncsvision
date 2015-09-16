varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;
uniform float textureWidth;
uniform float textureHeight;

void main(void)
{
        //tile *should* contain 1.0/bgl_TextureWidth and 1.0/bgl_TextureHeight but that doesn't work :(
        vec2 Tile = vec2(1.0/textureWidth,1.0/textureHeight );
        vec2 Blur = vec2(0.5,0.5);
        // Normalised position of current pixel in its tile
        vec2 tilePos = vec2(fract(texture_coordinate.x / Tile.x), fract(texture_coordinate.y / Tile.y));

        // Bottom-left of current tile
        vec2 p0 = vec2(floor(texture_coordinate.x / Tile.x) * Tile.x, floor(texture_coordinate.y / Tile.y) * Tile.y);
        // Bottom-left of tile to Left of current tile
        vec2 p1 = vec2(clamp(p0.x - Tile.x, 0.0, 1.0), p0.y);
        // Bottom-left of tile Below current tile
        vec2 p2 = vec2(p0.x, clamp(p0.y - Tile.y, 0.0, 1.0));
        // Bottom-left of tile Below and Left of current tile
        vec2 p3 = vec2(p1.x, p2.y);
        vec2 mixFactors;
        mixFactors.x =  min(tilePos.x / Blur.x, 1.0);
        mixFactors.y =  min(tilePos.y / Blur.y, 1.0);

        vec4 tmp1 = mix(texture2D(my_color_texture, p1+(Tile/2.0)), texture2D(my_color_texture, p0+(Tile/2.0)), mixFactors.x);
        vec4 tmp2 = mix(texture2D(my_color_texture, p3 +(Tile/2.0)), texture2D(my_color_texture, p2+(Tile/2.0)), mixFactors.x);
        gl_FragColor = mix(tmp2, tmp1, mixFactors.y);
}
