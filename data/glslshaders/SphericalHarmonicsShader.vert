/*
Spherical Harmonics code Paul Bourke, adapted from
http://local.wasp.uwa.edu.au/%7Epbourke/surfaces_curves/sphericalh/

GLSL assistance and general encouragement Memo

GLSL implementation alx @ toneburst, 2008
*/

/////////////////////
////  CONSTANTS  ////
/////////////////////

#define TWOPI 6.28318531
#define PI    3.14159265

/////////////////////
//// TWEAKABLES  ////
/////////////////////

// Pre-Transform controls
uniform vec4 TT_0;		// TT_0(X) = Pre-Scale X (range 0.0 > 1.0)
uniform vec4 TT_1;		// TT_1(Y) = Pre-Scale Y (range 0.0 > 1.0)
uniform vec4 TT_2;
uniform vec4 TT_3;		// TT_3(X),(Y) = Pre-Translate X,Y (range 0.0 > 1.0)

// Spherical Harmonics controls (range 0.0 to 10.0)
uniform float M0,M1,M2,M3,M4,M5,M6,M7;

// Light position
uniform vec3 LightPosition;
// Passes result of shading calculation to Fragment Shader
varying float colpos;
// The actual Spherical Harmonics formula (operates on Spherical coordinates)
vec3 sphericalHarmonics(float theta, float phi, float m0,float m1,float m2,float m3,float m4,float m5,float m6,float m7)
{
        vec3 point;
        float r = 0.0;
        r += pow(sin(m0*phi),m1);
        r += pow(cos(m2*phi),m3);
        r += pow(sin(m4*theta),m5);
        r += pow(cos(m6*theta),m7);
        point.x = r * sin(phi) * cos(theta);
        point.y = r * cos(phi);
        point.z = r * sin(phi) * sin(theta);
        return point;
}
/////////////////////
////  MAIN LOOP  ////
/////////////////////
void main()
{
        // Create pre-transform matrix from uniform vec4s
        mat4 TT = mat4(TT_0,TT_1,TT_2,TT_3);
        // Get vertex coordinates (cartesian)
        vec4 vertex = gl_Vertex;
        // Initial vertex position pre-transformed
        vertex = gl_ModelViewProjectionMatrix * vertex;
        // Spherical coordinates to send to Spherical Harmonics function
        float theta = (vertex.x + 0.5) * TWOPI;	// set range to 0 > TWOPI
        float phi = (vertex.y + 0.5) * PI;		// set range 0 > PI
        // Spherical Harmonics function
        vertex.xyz = sphericalHarmonics(theta, phi, M0, M1, M2, M3, M4, M5, M6, M7);
        // Shading calculation
        colpos = length(vertex.xyz  );
        // Transform vertex by modelview and projection matrices
        gl_Position = gl_ModelViewProjectionMatrix * vertex;

        // Forward current color and texture coordinates after applying texture matrix
        gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
