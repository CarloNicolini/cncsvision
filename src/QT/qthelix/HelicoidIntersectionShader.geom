#version 330
#extension GL_EXT_geometry_shader4 : enable
layout(triangles) in;
layout(line_strip) out;

uniform mat4 MVP;
uniform bool useIntersectionShader;
uniform float helicoidTheta;
uniform float helicoidRadius;
uniform float helicoidHeight;
float PI=3.141592653;

void emitIntersection(in vec4 a, in  float distA, in vec4 b, in float distB)
{
    if (sign(distA) * sign(distB) <= 0.0f && !(sign(distA) == 0 && sign(distB) == 0))
    {
        float fa = abs(distA);
        float fb = abs(distB);
        if ( fa+fb < PI*0.5)
        {
        gl_Position = MVP * ((fa * b + fb * a) / (fa + fb));
        EmitVertex();
        }
    }
}

float distToHelicoid(in vec3 p,in float helixRadius, in float helixHeight)
{
    vec3 offset = vec3(0.0,helicoidTheta,0.0);
    float helicoidC = helixHeight/(2*PI);
    float theta = (p.y -offset.y)/(helicoidC);
    float a = mod(theta - atan(p.z-offset.z, p.x-offset.x), 2*PI) - PI; // [-PI, PI[
    return a;
}

void main()
{
  if (useIntersectionShader)
  {
        float dist[3];
        for (int i=0; i<3; i++)
            dist[i] = distToHelicoid(gl_in[i].gl_Position.xyz,helicoidRadius,helicoidHeight);

        // Find the smallest i where vertex i is below and vertex i+1 is above the plane.
        ivec3 ijk = ivec3(0, 1, 2); // use swizzle to permute the indices
        for (int i=0; i < 3 && (dist[ijk.x] > 0 || dist[ijk.y] < 0); ijk=ijk.yzx, i++);

        emitIntersection(gl_in[ijk.x].gl_Position, dist[ijk.x], gl_in[ijk.y].gl_Position, dist[ijk.y]);
        emitIntersection(gl_in[ijk.y].gl_Position, dist[ijk.y], gl_in[ijk.z].gl_Position, dist[ijk.z]);
        emitIntersection(gl_in[ijk.z].gl_Position, dist[ijk.z], gl_in[ijk.x].gl_Position, dist[ijk.x]);
    }
    else
    {
    // This is a passthrough shader that receives gl_Position==gl_Vertex with no previous transformations, so the final gl_Position
    // is projectionMatrix*modelViewMatrix*gl_in[i].gl_Position and that is emitted
        int i=0;
        for (i=0; i<gl_in.length();i++)
        {
            gl_Position = MVP*(gl_in[i].gl_Position);

            EmitVertex();
        }
        EndPrimitive();
    }
}
