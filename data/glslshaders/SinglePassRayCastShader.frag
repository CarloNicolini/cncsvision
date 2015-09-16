uniform sampler3D Density;
const vec3 LightPosition = vec3(0.0, 0.0, 3.0);
const vec3 LightIntensity = vec3(15.0);
const float Absorption = 1.0;

float FocalLength=1.0/tan(45.0);
varying mat4 ModelView;

const vec2 WindowSize = vec2(800.0,800.0);
const vec3 RayOrigin = vec3(0.0,0.0,15.0);

// Const di base
const float maxDist = sqrt(2.0);
const int numSamples = 256;
const int numLightSamples = 16;
const float stepSize = maxDist/float(numSamples);
const float lscale = maxDist / float(numLightSamples);
const float densityFactor = 5.0;


bool IntersectBox(vec3 rayOrigin, vec3 rayDir, vec3 cubeMin, vec3 cubeMax, out float t0, out float t1)
{
    vec3 invR = 1.0 / rayDir;
    vec3 tbot = invR * (cubeMin-rayOrigin);
    vec3 ttop = invR * (cubeMax-rayOrigin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}

void main()
{
    vec3 rayDirection;
    rayDirection.xy = 2.0 * gl_FragCoord.xy / vec2(800.0,800.0) - 1.0;
    rayDirection.z = -FocalLength;
    rayDirection = (vec4(rayDirection, 0.0) * ModelView).xyz;

    vec3 eye = RayOrigin;
    vec3 eyeDirection = normalize(rayDirection);

    vec3 cubeMin = vec3(-1.0);
    vec3 cubeMax = vec3(1.0);

    float tnear, tfar;
    IntersectBox(eye,eyeDirection, cubeMin,cubeMax, tnear, tfar);
    if (tnear < 0.0) tnear = 0.0;

    vec3 rayStart = eye + eyeDirection * tnear;
    vec3 rayStop  = eye + eyeDirection * tfar;
    rayStart = 0.5 * (rayStart + 1.0);
    rayStop = 0.5 * (rayStop + 1.0);

    vec3 pos = rayStart;
    vec3 step = normalize(rayStop-rayStart) * stepSize;
    float travel = distance(rayStop, rayStart);
    float T = 1.0;
    vec3 Lo = vec3(0.0);

    for (int i=0; i < numSamples && travel > 0.0; ++i, pos += step, travel -= stepSize)
    {
        float density = texture3D(Density, pos).x * densityFactor;
        //if (density <= 0.0)
          //  continue;

        T *= 1.0-density*stepSize*Absorption;
        if (T <= 0.01)
            break;

        vec3 lightDir = normalize(LightPosition-pos)*lscale;
        /*
        float Tl = 1.0;
        vec3 lpos = pos + lightDir;
        for (int s=0; s < numLightSamples; ++s) {
            float ld = texture3D(Density, lpos).x;
            Tl *= 1.0-Absorption*stepSize*ld;
            if (Tl <= 0.01)
            lpos += lightDir;
        }
        */
        vec3 Li = LightIntensity;//*Tl;
        Lo += Li*T*density*stepSize;
    }

    gl_FragColor = vec4(Lo,1.0-T);
}
