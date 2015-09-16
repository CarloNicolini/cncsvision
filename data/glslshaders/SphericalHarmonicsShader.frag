// Base color
uniform vec4 Color;
// Lighting range. Range 0.1 > 1.0 (use exponential control)
uniform float LightRange;
// Shading calculation from Vertex Shader
varying float colpos;
// Shading lookup table input
uniform sampler2D LUT;
// Lookup y-position. Range 0.0 > 1.0
uniform float LUT_Y;

// Surface texture input
uniform sampler2D TileImg;
// Surface texture scale. Range 0.001 > 0.5
uniform vec2 Tile;
void main()
{
        // Fake lighting shading with Lookup Table
        float lookupX = clamp((1.0-LightRange) * colpos,0.1,0.9);
        // Lookup shade across x-axis of LUT at y-position set by LUT_Y input
        vec4 shade = texture2D(LUT, vec2(lookupX,LUT_Y));

        // Surface tiling texture
        vec2 xy = gl_TexCoord[0].xy;
        vec2 phase = fract(xy / Tile);
        vec4 texTile = texture2D(TileImg,phase);

        // Output color compute
        gl_FragColor = vec4(1.0,1.0,1.0,1.0); //Color * shade * texTile;
}
