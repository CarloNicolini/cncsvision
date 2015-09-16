varying vec3 LightIntensity;

void main()
{
    
    vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal);
    vec4 eyeCoords = gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1.0);
    vec3 s = normalize(vec3(gl_LightSource[0].position - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect( -s, tnorm );
    vec3 ambient = vec3(gl_LightSource[0].ambient*gl_FrontMaterial.ambient);
    
    float sDotN = max( dot(s,tnorm), 0.0 );
    vec3 diffuse = vec3(gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * sDotN);
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = gl_LightSource[0].specular * gl_FrontMaterial.specular *
                pow( max( dot(r,v), 2.0 ), gl_FrontMaterial.shininess );
    
    LightIntensity = ambient + diffuse + spec;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}
