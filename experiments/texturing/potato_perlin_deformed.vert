uniform mat4 GL_ModelViewProjectionMatrix;
uniform mat4 GL_ModelViewMatrix;
uniform mat3 GL_NormalMatrix; // is the transpose of inverse of the 3x3 linear part of GL_ModelViewMatrix
uniform vec4 GL_LightPosition;

const float eps = 1E0;
const vec3 X = vec3(1.0,0.0,0.0);
const vec3 Y = vec3(0.0,1.0,0.0);
const vec3 Z = vec3(0.0,0.0,1.0);

uniform float normalScale;
uniform float vertexScale;
uniform float seed;
uniform float zScale;

varying vec3 vVaryingLightDir;
varying vec3 vVaryingNormal;
varying vec3 texture_coordinate;

float Perlin3D( vec3 P)
{
    // https://github.com/BrianSharpe/Wombat/blob/master/Perlin3D.glsl
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;
    vec3 Pf_min1 = Pf - 1.0;
    // clamp the domain
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
    // calculate the hash
    vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    vec3 SOMELARGEfloatS = vec3( 635.298681, 682.357502, 668.926525 )+seed;
    vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
    vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEfloatS + Pi.zzz * ZINC ) );
    vec3 highz_mod = vec3( 1.0 / ( SOMELARGEfloatS + Pi_inc1.zzz * ZINC ) );
    vec4 hashx0 = fract( Pt * lowz_mod.xxxx );
    vec4 hashx1 = fract( Pt * highz_mod.xxxx );
    vec4 hashy0 = fract( Pt * lowz_mod.yyyy );
    vec4 hashy1 = fract( Pt * highz_mod.yyyy );
    vec4 hashz0 = fract( Pt * lowz_mod.zzzz );
    vec4 hashz1 = fract( Pt * highz_mod.zzzz );
    // calculate the gradients
    vec4 grad_x0 = hashx0 - 0.49999;
    vec4 grad_y0 = hashy0 - 0.49999;
    vec4 grad_z0 = hashz0 - 0.49999;
    vec4 grad_x1 = hashx1 - 0.49999;
    vec4 grad_y1 = hashy1 - 0.49999;
    vec4 grad_z1 = hashz1 - 0.49999;
    vec4 grad_results_0 = inversesqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
    vec4 grad_results_1 = inversesqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );
    // Classic Perlin Interpolation
    vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
    vec4 res0 = mix( grad_results_0, grad_results_1, blend.z );
    vec4 blend2 = vec4( blend.xy, vec2( 1.0 - blend.xy ) );
    float final = dot( res0, blend2.zxzx * blend2.wwyy );
    return ( final * 1.1547005383792515290182975610039 ); // scale things to a strict -1.0->1.0 range *= 1.0/sqrt(0.75)
}

void main(void)
{
    vec4 V = gl_Vertex;
    vec3 O = (V.xyz)+ gl_Normal.xyz*normalScale*Perlin3D(V.xyz);
    O.z*=zScale;

    float dPx = normalScale*(Perlin3D(O+eps*X)-Perlin3D(O-eps*X))/(2.0*eps);
    float dPy = normalScale*(Perlin3D(O+eps*Y)-Perlin3D(O-eps*Y))/(2.0*eps);
    float dPz = normalScale*(Perlin3D(O+eps*Z)-Perlin3D(O-eps*Z))/(2.0*eps);

    float dfxdx = 1.0;
    float dfxdy = 0.0;
    float dfxdz = 0.0;

    float dfydx = 0.0;
    float dfydy = 1.0;
    float dfydz = 0.0;

    float dfzdx = dPx;
    float dfzdy = dPy;
    float dfzdz = 1.0+dPz;

    vec3 tangent;
    vec3 T1 = cross(gl_Normal.xyz,Z);
    vec3 T2 = cross(gl_Normal.xyz,Y);
    if (length(T2) > length(T1))
        tangent = T2;
    else
        tangent = T1;
    vec3 binormal = cross(gl_Normal.xyz,tangent);

    mat3 J = mat3(dfxdx,dfydx,dfzdx,
                  dfxdy,dfydy,dfzdy,
                  dfzdx,dfydz,dfzdz);

    vec3 normal = normalize(cross(J*tangent,J*binormal));

    V.xyz= O*vertexScale;
    // Texture coordinates have to be in [0,1] range
    texture_coordinate = O/(1.0+normalScale);
    vVaryingNormal =  GL_NormalMatrix * normal;

    vec4 vPosition4 = GL_ModelViewMatrix*V;
    vec3 vPosition3 = vPosition4.xyz/vPosition4.w;

    vVaryingLightDir = normalize(GL_LightPosition-vPosition3);
    gl_Position = GL_ModelViewProjectionMatrix * V;
    gl_ClipVertex = gl_ModelViewMatrix * V;
}
