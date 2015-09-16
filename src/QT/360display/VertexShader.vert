uniform vec3 ProjectorCenter;
uniform vec3 ReflectedProjector;
uniform vec3 mirrorNormal;
uniform vec3 mirrorCenter;
uniform float viewerDistance;
uniform float viewerHeight;
uniform mat4 projectorModelViewProjectionMatrix;
varying vec4 fragColor;

vec3 getRayCylinderIntersection(  vec3 rayDirection,  vec3 rayOrigin, float cylinderRadius )
{
    float a = rayDirection.x*rayDirection.x+rayDirection.z*rayDirection.z;
    float b = 2.0*(rayDirection.x*rayOrigin.x+rayOrigin.z*rayDirection.z);
    float c = rayOrigin.x*rayOrigin.x+rayOrigin.z*rayOrigin.z-cylinderRadius*cylinderRadius;
    float t = (-b+sqrt(b*b-4.0*a*c))/(2.0*a);

    return rayDirection*t+rayOrigin;
}

vec3 getRayPlaneIntersection( vec3 rayDirection,  vec3 rayOrigin,  vec3 planeNormal,  vec3 planeOrigin)
{
    float offset = -dot(planeNormal,planeOrigin);
    float t = -(offset+dot(planeNormal,rayOrigin) )/ dot(planeNormal,rayDirection);
    return rayDirection*t+rayOrigin;
}

void main(void)
{
    vec3 Q = gl_Vertex.xyz;
    // define ray from reflected projector position P’ to vertex Q
    vec3 PQ = normalize(Q - ReflectedProjector);
    // compute intersection of ray PQ with vertical cylinder with radius d to find view position V’
    vec3 V = getRayCylinderIntersection(PQ,Q,viewerDistance);
    // set correct viewer height
    V.y = viewerHeight;
    // define ray from ideal viewing position V’ to vertex Q
    vec3 VQ = (Q - V);
    //n.xyz=vec3(1,0,0);
    // compute intersection ray VQ with mirror plane to find point M
    vec3 M = getRayPlaneIntersection(VQ,V,mirrorNormal, mirrorCenter);

    vec4 outputQ = ( projectorModelViewProjectionMatrix* vec4(M.xyz,1.0) ); // project M into projector

    // keep the existing vertex color, recompute depth in based on distance from V’
    outputQ.z = length(V -Q) / (2.0 * length(V - M));
    gl_Position = outputQ;
    //gl_Position = projectorModelViewProjectionMatrix*vec4(M.xyz,1.0);
    fragColor = vec4(outputQ.z);
}
