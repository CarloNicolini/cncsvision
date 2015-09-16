attribute vec2 v_texcoords;
attribute vec3 v_tangent;

varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec2 texCoords;
varying mat3 localSurface2World; // mapping from local surface coordinates to world coordinates

void main()
{
    position = gl_ModelViewProjectionMatrix*gl_Vertex;

    // the signs and whether tangent is in localSurface2View[1] or
    // localSurface2View[0] depends on the tangent attribute, texture
    // coordinates, and the encoding of the normal map
    mat3 m3x3invTransp = transpose(inverse(mat3(gl_ModelViewMatrix)));
    localSurface2World[0] = normalize(vec3(gl_ModelViewMatrix * vec4(v_tangent, 0.0)));
    localSurface2World[2] = normalize(m3x3invTransp * gl_Normal);
    localSurface2World[1] = normalize(cross(localSurface2World[2], localSurface2World[0]));

    texCoords = v_texcoords;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

