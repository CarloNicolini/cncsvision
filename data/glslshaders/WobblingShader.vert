attribute vec4 tangent;

uniform float noiseRate;
uniform vec3 lightPos;
uniform float wripplephase;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vBinormal;
varying vec3 vViewVec;
varying vec3 vPos;

varying vec4 lightDir;
varying vec2 vUv;

void main()
{
    vec4 mvPosition = gl_ModelViewMatrix * vec4( position, 1.0 );

    vTangent     = normalize( normalMatrix * tangent.xyz );
    vNormal      = normalize( normalMatrix * normal );
    vBinormal    = cross( vNormal, vTangent ) * tangent.w;
    vBinormal    = normalize( vBinormal );

    vUv          = uv * noiseRate;
    vViewVec.xyz = mvPosition.xyz;
    vPos         = vec3(uv,1.0) * noiseRate;

    vec4 lPosition      = modelViewMatrix * vec4( lightPos, 1.0 );
    lightDir            = normalize(vec4( lPosition.xyz - mvPosition.xyz , 1.0));

    // apply wobble

    vec4 bVertexPosition    = mvPosition;
    float frequency         = 0.005;
    float wrippleScale      = 50.4;
    bVertexPosition.x += cos(bVertexPosition.y*frequency + wripplephase) * wrippleScale;
    bVertexPosition.y += sin(bVertexPosition.x*frequency - wripplephase) * wrippleScale;
    bVertexPosition.z += cos(bVertexPosition.y*frequency - wripplephase) * wrippleScale;

    gl_Position     = projectionMatrix * bVertexPosition;
}

