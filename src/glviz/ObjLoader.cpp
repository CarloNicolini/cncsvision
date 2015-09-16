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

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include "ObjLoader.h"

using namespace std;

/**
* \ingroup GLVisualization
* \brief Default constructor
*
**/
ObjLoader::ObjLoader()
{
    center << 0,0,0;
    minX =minY = minZ = std::numeric_limits<float>::max();
    maxX =maxY = maxZ = std::numeric_limits<float>::min();

    fillColor << 1,1,1;
    lineColor<< 0,0,0;

    this->TotalConnectedTriangles = 0;
    this->TotalConnectedPoints = 0;
    norm[0]=norm[1]=norm[2]=0.0;
    Faces_Triangles=normals=vertexBuffer=NULL;
}

/**
 * @brief ObjLoader::ObjLoader
 * @param filename
 */
ObjLoader::ObjLoader(char *filename)
{
    center << 0,0,0;
    minX =minY = minZ = std::numeric_limits<float>::max();
    maxX =maxY = maxZ = std::numeric_limits<float>::min();

    fillColor << 1,1,1;
    lineColor<< 0,0,0;

    this->TotalConnectedTriangles = 0;
    this->TotalConnectedPoints = 0;
    norm[0]=norm[1]=norm[2]=0.0;
    Faces_Triangles=normals=vertexBuffer=NULL;
    this->load(filename);
}

/**
* \ingroup GLVisualization
* \brief Destructor, dispose all the memory occupied by the 3d mesh object.
**/
ObjLoader::~ObjLoader()
{  release();
}

/**
* \ingroup GLVisualization
* \brief Compute the normals of a triangle surface.
* @param coord1 pointer to first vertex coordinates
* @param coord2 pointer to second vertex coordinates
* @param coord3 pointer to third vertex coordinates
* @return pointer to normal vector of the surface
**/
float* ObjLoader::calculateNormal( float *coord1, float *coord2, float *coord3 )
{  // calculate Vector1 and Vector2
    float va[3], vb[3], vr[3], val;
    va[0] = coord1[0] - coord2[0];
    va[1] = coord1[1] - coord2[1];
    va[2] = coord1[2] - coord2[2];
    
    vb[0] = coord1[0] - coord3[0];
    vb[1] = coord1[1] - coord3[1];
    vb[2] = coord1[2] - coord3[2];
    
    // cross product
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = vb[0] * va[2] - va[0] * vb[2];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];
    
    // normalization factor
    val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
    
    norm[0] = vr[0]/val;
    norm[1] = vr[1]/val;
    norm[2] = vr[2]/val;
    
    return norm;
}

void ObjLoader::centerToUnitBoundingBox()
{
    for (long int i=0; i<TotalConnectedTriangles;i++)
    {
        for (int k=0; k<3;k++)
            Faces_Triangles[i+k]-=this->center[k];
    }

    float sizeX=maxX-minX;
    float sizeY=maxY-minY;
    float sizeZ=maxZ-minZ;

    float sizeScale =std::max( std::max(sizeX,sizeY),sizeZ);
    for (long int i=0; i<TotalConnectedTriangles;i++)
    {
        for (int k=0; k<3;k++)
            Faces_Triangles[i+k] /= sizeScale;
    }
}

/**
* \ingroup GLVisualization
* \brief Compute the normals of a triangle surface.
* @param filename string representing the full name of obj mesh file
* @return 0 always
**/
int ObjLoader::load(char* filename)
{
    center << 0,0,0;
    minX =minY = minZ = std::numeric_limits<float>::max();
    maxX =maxY = maxZ = std::numeric_limits<float>::min();
    string line("");
    ifstream objFile (filename);
    if (!objFile)
    {
        cerr << "File " << filename << " does not exist" << endl;
        //exit(0);
    }
    if (objFile.is_open()) // If obj file is open, continue
    {  cerr << "File " << string(filename) << " loaded successfully" << endl;
        objFile.seekg (0, ios::end);                             // Go to end of the file,
        long fileSize = objFile.tellg();                         // get file size
        objFile.seekg (0, ios::beg);                             // we'll use this to register memory for our 3d model
        
        vertexBuffer = (float*) calloc (fileSize,sizeof(float));                     // Allocate memory for the verteces
        Faces_Triangles = (float*) calloc(fileSize,sizeof(float));        // Allocate memory for the triangles
        normals  = (float*) calloc(fileSize,sizeof(float));               // Allocate memory for the normals

        int triangle_index = 0;                                  // Set triangle index to zero
        int normal_index = 0;                                    // Set normal index to zero
        
        while (! objFile.eof() )                                 // Start reading file data
        {  getline (objFile,line);                               // Get line from file
            switch ( line.c_str()[0] )
            {  case 'o':
                cerr << "Objects not supported directly, continuing..." << endl;
                continue;
                break;
                
            case 'g':
                cerr << "Groups not supported directly, continuing..." << endl;
                continue;
                break;
                
            case 's':
                cerr << "Smooth shading across polygons not supported directly, continuing..." << endl;
                continue;
                break;
                
            case 'm':
                cerr << "Material specifications not supported, continuing..." << endl;
                continue;
                break;
            case '#':
                continue;
            case 'v':
            {  if (line.c_str()[1] == 'n')
                    continue;
                line[0] = ' ';                                  // Set first character to 0. This will allow us to use sscanf
                
                sscanf(line.c_str(),"%f %f %f ",                   // Read floats from the line: v X Y Z
                       &vertexBuffer[TotalConnectedPoints],
                       &vertexBuffer[TotalConnectedPoints+1],
                        &vertexBuffer[TotalConnectedPoints+2]);
                
                minX = std::min(minX,vertexBuffer[TotalConnectedPoints]);
                maxX = std::max(maxX,vertexBuffer[TotalConnectedPoints]);

                minY = std::min(minY,vertexBuffer[TotalConnectedPoints+1]);
                maxY = std::max(maxY,vertexBuffer[TotalConnectedPoints+1]);

                minZ = std::min(minZ,vertexBuffer[TotalConnectedPoints+2]);
                maxZ = std::max(maxZ,vertexBuffer[TotalConnectedPoints+2]);

                center+= Eigen::Vector3f(vertexBuffer[TotalConnectedPoints],vertexBuffer[TotalConnectedPoints+1],vertexBuffer[TotalConnectedPoints+2]);
                TotalConnectedPoints += POINTS_PER_VERTEX;               // Add 3 to the total connected points
            }
                break;
                
            case 'f':
            {  line[0] = ' ';                                  // Set first character to 0. This will allow us to use sscanf
                int vertexNumber[4] = { 0, 0, 0 };
                if ( line.find("/")!=string::npos )
                {
                    int junk=0;
                    replace(line.begin(),line.end(),'/',' ');
                    sscanf(line.c_str(),"%i%i%i%i%i%i",                        // Read integers from the line:  f 1 2 3
                           &vertexNumber[0],
                            &junk,                            // First point of our triangle. This is an
                            &vertexNumber[1],
                            &junk,                            // pointer to our vertexBuffer list
                            &vertexNumber[2],
                            &junk );
                }
                else
                {  sscanf(line.c_str(),"%i%i%i",                   // Read integers from the line:  f 1 2 3
                          &vertexNumber[0],                            // First point of our triangle. This is an
                            &vertexNumber[1],                            // pointer to our vertexBuffer list
                            &vertexNumber[2] );                             // each point represents an X,Y,Z.
                }
                vertexNumber[0] -= 1;                              // OBJ file starts counting from 1
                vertexNumber[1] -= 1;                              // OBJ file starts counting from 1
                vertexNumber[2] -= 1;                              // OBJ file starts counting from 1

                /*
               * Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z).
               * The vertexBuffer contains all verteces
               * The triangles will be created using the verteces we read previously
               */
                
                int tCounter = 0;
                for (int i = 0; i < POINTS_PER_VERTEX; i++)
                {
                    Faces_Triangles[triangle_index + tCounter       ] = vertexBuffer[3*vertexNumber[i] ];
                    Faces_Triangles[triangle_index + tCounter +1 ] = vertexBuffer[3*vertexNumber[i]+1 ];
                    Faces_Triangles[triangle_index + tCounter +2 ] = vertexBuffer[3*vertexNumber[i]+2 ];
                    tCounter += POINTS_PER_VERTEX;
                }
                
                /*
                * Calculate all normals, used for lighting
                */
                float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
                float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
                float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
                float *norm = this->calculateNormal( coord1, coord2, coord3 );
                
                tCounter = 0;
                for (int i = 0; i < POINTS_PER_VERTEX; i++)
                {  normals[normal_index + tCounter ] = norm[0];
                    normals[normal_index + tCounter +1] = norm[1];
                    normals[normal_index + tCounter +2] = norm[2];
                    tCounter += POINTS_PER_VERTEX;
                }
                
                triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
                normal_index += TOTAL_FLOATS_IN_TRIANGLE;
                TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;
            }
                break;
            }
        }
        objFile.close(); // Close OBJ file
    }
    else
    {
        throw std::runtime_error("Unable to open file " + string(filename) );
    }

    center/=TotalConnectedPoints;
    return 0;
}

/**
* \ingroup GLVisualization
* \brief Clear the memory
**/
void ObjLoader::release()
{  
    if (this->Faces_Triangles)
        free(this->Faces_Triangles);
    if (this->normals)
        free(this->normals);
    if (this->vertexBuffer)
        free(this->vertexBuffer);
}

/**
* \ingroup GLVisualization
* \brief Draw the mesh.
**/
void ObjLoader::draw(GLuint mode)
{
    if (mode== GL_FILL || mode==GL_LINE || mode==GL_POINT )
    {
        glPolygonMode( GL_FRONT_AND_BACK, mode );
        glEnableClientState(GL_VERTEX_ARRAY);                 // Enable vertex arrays
        glEnableClientState(GL_NORMAL_ARRAY);                 // Enable normal arrays
        glVertexPointer(3,GL_FLOAT,   0,Faces_Triangles);     // Vertex Pointer to triangle array
        glNormalPointer(GL_FLOAT, 0, normals);                // Normal pointer to normal array
        glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);     // Draw the triangles
        glDisableClientState(GL_VERTEX_ARRAY);                // Disable vertex arrays
        glDisableClientState(GL_NORMAL_ARRAY);                // Disable normal arrays
    }
	/*
    if (mode == ( GL_FILL | GL_LINE ) )
    {
        glColor3fv(lineColor.data());
        glPolygonOffset(0.1,0.1);
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glEnableClientState(GL_VERTEX_ARRAY);                 // Enable vertex arrays
        glEnableClientState(GL_NORMAL_ARRAY);                 // Enable normal arrays
        glVertexPointer(3,GL_FLOAT,   0,Faces_Triangles);     // Vertex Pointer to triangle array
        glNormalPointer(GL_FLOAT, 0, normals);                // Normal pointer to normal array
        glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);     // Draw the triangles
        glDisableClientState(GL_VERTEX_ARRAY);                // Disable vertex arrays
        glDisableClientState(GL_NORMAL_ARRAY);                // Disable normal arrays

        glColor3fv(fillColor.data());
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glEnableClientState(GL_VERTEX_ARRAY);                 // Enable vertex arrays
        glEnableClientState(GL_NORMAL_ARRAY);                 // Enable normal arrays
        glVertexPointer(3,GL_FLOAT,   0,Faces_Triangles);     // Vertex Pointer to triangle array
        glNormalPointer(GL_FLOAT, 0, normals);                // Normal pointer to normal array
        glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);     // Draw the triangles
        glDisableClientState(GL_VERTEX_ARRAY);                // Disable vertex arrays
        glDisableClientState(GL_NORMAL_ARRAY);                // Disable normal arrays
    }
	*/
//    else
//        throw std::runtime_error("Non valid mode can only be GL_FILL or GL_LINE or GL_POINT");
}

void ObjLoader::drawOnlyVertices()
{
	this->draw(GL_POINT);
}

float* ObjLoader::getNormals()
{
    return normals;
}

float* ObjLoader::getVertices()
{
    return vertexBuffer;
}

float* ObjLoader::getTriangles()
{
    return Faces_Triangles;
}

long ObjLoader::getTotalVertices()
{
    return TotalConnectedPoints;
}

