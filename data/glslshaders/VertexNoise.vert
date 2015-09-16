// vertexnoise.vert: Vertex shader for warping the geometry with noise.
// author: Philip Rideout
// Copyright (c) 2005-2006: 3Dlabs, Inc.
// See 3Dlabs-License.txt for license information


vec4 surfaceColor=vec4(1.0);
vec3 offset = vec3(0.0,0.0,0.0);
float scaleIn=10.0;
uniform float scaleOut=15.0;
varying vec4 Color;

void main(void)
{
    vec3 normal = gl_Normal;
    vec3 vertex = gl_Vertex.xyz + noise3(offset + gl_Vertex.xyz * scaleIn) * scaleOut;

    normal = normalize(gl_NormalMatrix * normal);
    vec3 position = vec3(gl_ModelViewMatrix * vec4(vertex,1.0));
    vec3 lightVec   = normalize(gl_LightSource[0].position - position);
    float diffuse   = max(dot(lightVec, normal), 0.0);
    if (diffuse < 0.125)
        diffuse = 0.125;
    Color = vec4(surfaceColor.rgb * diffuse, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex,1.0);
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
