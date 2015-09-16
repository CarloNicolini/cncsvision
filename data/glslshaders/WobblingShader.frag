uniform sampler2D noise;
uniform vec3 gloss;
uniform vec3 color;
uniform float noiseScale;
uniform vec3 diffuseColor;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vBinormal;
varying vec3 vViewVec;

varying vec2 vUv;
varying vec3 vPos;

varying vec4 lightDir;

void main() {

    vec3 viewVec        = normalize(vViewVec);
    vec3 oglLightDir    = vec3(lightDir.x, lightDir.y, -lightDir.z);
    float angle         = noiseScale * ( texture2D(noise, vUv) - 0.5).x;

    float cosA, sinA;
    sinA = sin(angle);
    cosA = cos(angle);

    vec3 tang =  sinA * vTangent + cosA * vBinormal;

    // anisotropic lighting

    float diffuse = clamp( dot( -oglLightDir.xyz, vNormal ), 0.0, 1.0 );

    float cs = -dot(viewVec, tang);
    float sn = sqrt(1.0 - cs * cs);
    float cl =  dot(oglLightDir.xyz, tang);
    float sl = sqrt(1.0 - cl * cl);

    float specular = pow( clamp( (cs * cl + sn * sl), 0.0, 1.0 ), 32.0);

    vec4 color4 = vec4(color, 0.0);
    vec4 gloss4 = vec4(gloss, 0.0);

    vec4 total  = diffuse * color4 + gloss4 * specular;
    gl_FragColor= vec4(total.rgb, 1.0);
}
