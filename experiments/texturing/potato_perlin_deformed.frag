uniform vec4 GL_AmbientColor;
uniform vec4 GL_DiffuseColor;
uniform vec3 GL_SpecularColor;
uniform float GL_Shininess;

uniform float normalScale;
uniform float vertexScale;

varying vec3 vVaryingNormal;
varying vec3 vVaryingLightDir;

uniform sampler3D my_color_texture;
varying vec3 texture_coordinate;


void main(void)
{
    float diff = max(0.0,dot(normalize(vVaryingNormal),vVaryingLightDir));
    vec4 vFragColor = diff*GL_DiffuseColor + GL_AmbientColor;
    gl_FragColor =  vFragColor + length(vFragColor.rgb)*(texture3D(my_color_texture, texture_coordinate));

}
