uniform  highp vec3 reflectedProjector;
uniform  highp vec3 mirrorNormal;
uniform  highp vec3 mirrorCenter;
uniform  highp float viewerDistance;
uniform  highp float viewerHeight;
uniform float scale;

vec3 getRayCylinderIntersection(  vec3 rayDirection,  vec3 rayOrigin, float cylinderRadius )
{
    float a = rayDirection.x*rayDirection.x+rayDirection.z*rayDirection.z;
    float b = 2.0*(rayDirection.x*rayOrigin.x+rayOrigin.z*rayDirection.z);
    float c = rayOrigin.x*rayOrigin.x+rayOrigin.z*rayOrigin.z-cylinderRadius*cylinderRadius;
    float t = (-b+sqrt(b*b-4.0*a*c))/(2.0*a);

    return rayDirection*t+rayOrigin;
}

vec3 getRayPlaneIntersection( vec3 D,  vec3 E,  vec3 N,  vec3 Q)
{
    return (dot(N,Q-E)/dot(N,D))*D+E;
}

void main(void)
{
    vec3 Q = gl_Vertex.xyz*scale;
    // define ray from reflected projector position P’ to vertex Q
    vec3 PQ = normalize(Q - reflectedProjector);
    // compute intersection of ray PQ with vertical cylinder with radius d to find view position V’
    vec3 V = getRayCylinderIntersection(PQ,Q,viewerDistance);
    // set correct viewer height
    V.y = viewerHeight;
    // define ray from ideal viewing position V’ to vertex Q
    vec3 VQ = normalize(Q - V);
    //n.xyz=vec3(1,0,0);
    // compute intersection ray VQ with mirror plane to find point M
    vec3 M = getRayPlaneIntersection(VQ,V,mirrorNormal, mirrorCenter);

    vec4 outputQ = ( gl_ModelViewProjectionMatrix*vec4(M.xyz,1.0) ); // project M into projector
    //vec4 outputQ = ( MV*Proj*vec4(M.xyz,1.0) ); // project M into projector

    // keep the existing vertex color, recompute depth in based on distance from V’
    outputQ.z = length(V -Q) / (2.0*length(V - M));

    gl_Position = outputQ;
}

