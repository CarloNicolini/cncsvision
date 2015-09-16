// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#ifndef _OBJLOADER_H
#define _OBJLOADER_H_

#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9
#include <Eigen/Core>
#include "IncludeGL.h"

/**
 * \class ObjLoader
 * \ingroup GLVisualization
 * \brief This class is intended to load and show complex 3D meshes in the obj format.
 * The mesh MUST be triangulated and saved in .obj format.
 *
 * The .obj format lists the mesh AT LEAST the following way:
 * - List of vertices "v" x y z
 * - List of triangle f v1 v2 v3
 * Where x,y,z are the coordinates of the vertex and v1,v2,v3 are the indices of vertices defining the face f
 * The format Vertex/normal is also accepted
 * One must put two slashes after the vertex index before putting the normal index.
 * f v1//vn1 v2//vn2 v3//vn3
 * For example
 * f 4289//2567 4290//2567 5826//2567
 * Here vertex normal are ignored and computed separately.
 *
 * An example of valid file
 * \code
 # Blender3D v249 OBJ File:
# www.blender3d.org
mtllib angel2.mtl
o angel
v -0.504451 0.930309 0.063363
v -0.505382 0.927728 0.061753
v -0.507210 0.927071 0.063346
f 100864 100863 100883
f 65706 100938 100939
f 13263 13262 100946
f 41387 100947 100948
f 13263 100946 100950
f 63485 181425 100952
f 100828 100827 100953
f 144179 100949 100954
 * \endcode
 **/

#include <vector>
using std::vector;

class ObjLoader
{
public:
   ObjLoader();
   ObjLoader(char *filename);
   ~ObjLoader();
   int load(char *filename); // Loads the model
   void draw(GLuint mode=GL_FILL); // Draws the model on the screen
   void drawOnlyVertices();
   float *getTriangles();
   float *getNormals();
   float *getVertices();
   long getTotalVertices();
   void centerToUnitBoundingBox();
private:
   float norm[3];
   float* calculateNormal( float* coord1,float* coord2,float* coord3 );
   void release();            // Release the model

   float minX,maxX,minY,maxY,minZ,maxZ;
   Eigen::Vector3f center;
   float* normals;                     // Stores the normals
   float* Faces_Triangles;               // Stores the triangles
   float* vertexBuffer;             // Stores the points which make the object
   long TotalConnectedPoints;          // Stores the total number of connected verteces
   long TotalConnectedTriangles;       // Stores the total number of connected triangles
   Eigen::Matrix<GLfloat,3,1> fillColor;
   Eigen::Matrix<GLfloat,3,1> lineColor;
};
#endif
