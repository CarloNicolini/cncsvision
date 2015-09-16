varying vec3 Normal;
varying vec3 EyeDir;
uniform samplerCube cubeMap;
varying vec3 LightIntensity;



varying vec3 diffuseLight;
varying vec3 specularLight;



void main()
{



    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    Normal = gl_NormalMatrix * gl_Normal;
    EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);



    vec3 s = normalize(vec3(gl_LightSource[0].position - EyeDir));
    vec3 v = normalize(EyeDir);
    vec3 r = reflect( s, Normal );
    vec3 ambient = vec3(gl_LightSource[0].ambient*gl_FrontMaterial.ambient);



    float sDotN = max( dot(s,Normal), 0.0 );
    vec3 diffuse = vec3(gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * sDotN);
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = gl_LightSource[0].specular * gl_FrontMaterial.specular * pow( max( dot(r,v), 0.0 ), gl_FrontMaterial.shininess );

    diffuseLight = diffuse;
    specularLight = spec;
    LightIntensity = ambient + diffuse + spec;
    /*
    float c1 = gl_LightSource[0].constantAttenuation;
    float c2 = gl_LightSource[0].linearAttenuation;
    float c3 = gl_LightSource[0].quadraticAttenuation;



    float lightDistance = length( vec3(gl_LightSource[0].position.xyz - gl_Vertex.xyz));
    float fatt = min( 1.0/ (c1+ c2*lightDistance + c3*lightDistance*lightDistance ),0.0);
    LightIntensity *= fatt;
    */
}


