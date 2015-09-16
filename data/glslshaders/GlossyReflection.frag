varying vec4 position;
// position of the vertex (and fragment) in view space
varying vec3 varyingNormalDirection;
// surface normal vector in view space
varying vec4 texCoords; // interpolated texture coordinates
uniform sampler2D textureUnit;

void main()
{
    vec3 normalDirection = normalize(varyingNormalDirection);
    vec3 viewDirection = -normalize(vec3(position));
    vec3 lightDirection;
    float attenuation;

    vec2 longitudeLatitude = vec2(
                (atan(texCoords.y, texCoords.x)/3.1415926+1.0)*0.5,
                1.0 - acos(texCoords.z) / 3.1415926);
    // unusual processing of texture coordinates

    vec4 textureColor =
            texture2D(textureUnit, longitudeLatitude);

    if (0.0 == gl_LightSource[0].position.w)
        // directional light?
    {
        attenuation = 1.0; // no attenuation
        lightDirection =
                normalize(vec3(gl_LightSource[0].position));
    }
    else // point light or spotlight (or other kind of light)
    {
        vec3 positionToLightSource =
                vec3(gl_LightSource[0].position - position);
        float distance = length(positionToLightSource);
        attenuation = 1.0 / distance; // linear attenuation
        lightDirection = normalize(positionToLightSource);

        if (gl_LightSource[0].spotCutoff <= 90.0) // spotlight?
        {
            float clampedCosine = max(0.0, dot(-lightDirection,
                                               gl_LightSource[0].spotDirection));
            if (clampedCosine < gl_LightSource[0].spotCosCutoff)
                // outside of spotlight cone?
            {
                attenuation = 0.0;
            }
            else
            {
                attenuation = attenuation * pow(clampedCosine,
                                                gl_LightSource[0].spotExponent);
            }
        }
    }

    vec3 ambientLighting = vec3(gl_LightModel.ambient)
            * vec3(textureColor);

    vec3 diffuseReflection = attenuation
            * vec3(gl_LightSource[0].diffuse) * vec3(textureColor)
            * max(0.0, dot(normalDirection, lightDirection));

    vec3 specularReflection;
    if (dot(normalDirection, lightDirection) < 0.0)
        // light source on the wrong side?
    {
        specularReflection = vec3(0.0, 0.0, 0.0);
        // no specular reflection
    }
    else // light source on the right side
    {
        specularReflection = attenuation
                * vec3(gl_LightSource[0].specular)
                * vec3(gl_FrontMaterial.specular)
                * (1.0 - textureColor.a)
                // for usual gloss maps: "* textureColor.a"
                * pow(max(0.0, dot(reflect(-lightDirection,
                                           normalDirection), viewDirection)),
                      gl_FrontMaterial.shininess);
    }

    gl_FragColor = vec4(ambientLighting + diffuseReflection + specularReflection, texCoords.a);
}
