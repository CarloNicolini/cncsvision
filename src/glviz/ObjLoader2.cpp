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
#include <string>
#include <sstream>
#include "ObjLoader2.h"
#include "Util.h"
#include "GLUtils.h"

using namespace std;
/**
 * @brief ObjLoader2::ObjLoader2
 */
ObjLoader2::ObjLoader2()
{
    fillColor << 1.0,1.0,1.0,1.0;
    lineColor << 0.3,0.3,0.3,0.0;
    this->areGLBuffersInitialized=false;
}

ObjLoader2::~ObjLoader2()
{
    for (unsigned int i=0; i<this->shapes.size();i++)
    {
        glDeleteBuffers(1,&vertexBufferObjectIDs[i]);
        glDeleteBuffers(1,&indexBufferObjectIDs[i]);
    }
}

/**
 * @brief ObjLoader2::load
 * @param filename
 * @return
 */
bool ObjLoader2::load(const string &filename, const string &pathname)
{
    std::string err = tinyobj::LoadObj(this->shapes,filename.c_str(),pathname.c_str());
    if ( !err.empty() )
    {
        throw std::runtime_error (err);
    }
    return 0;
}

/**
 * @brief ObjLoader2::ObjLoader2
 * @param filename
 */
ObjLoader2::ObjLoader2(const string &filename)
{
    this->load(filename);
}

/**
 * @brief ObjLoader2::initializeBuffers
 */
void ObjLoader2::initializeBuffers()
{
    if (shapes.empty())
    {
        std::string errorMessage("Empty model, can't initialize OpenGL buffers with empty data");
        cerr << errorMessage << endl;
        throw std::runtime_error(errorMessage);
    }

    for (unsigned int i=0; i<shapes.size(); i++)
    {
        GLuint vertexBufferObjectId=0,indexBufferObjectId=0;
        // Create vertex buffer objects, you need to delete them when program exits
        // Try to put both vertex coords array, vertex normal array  in the same buffer object.
        // glBufferDataARB with NULL pointer reserves only memory space.
        // Copy actual data with 2 calls of glBufferSubDataARB, one for vertex coords and one for normals.
        // target flag is GL_ARRAY_BUFFER_ARB, and usage flag is GL_STATIC_DRAW_ARB
        glGenBuffersARB(1, &vertexBufferObjectId);
        // Do the vertex buffer object operations
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferObjectId);
        //allocate two blocks, the first part is for the vertices, the second for the normals
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vectorsizeof<GLfloat>(shapes.at(i).mesh.positions)+vectorsizeof<GLfloat>(shapes.at(i).mesh.normals), 0, GL_STATIC_DRAW_ARB);

        glGenBuffersARB(1, &indexBufferObjectId);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferObjectId);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vectorsizeof<unsigned int>(shapes.at(i).mesh.indices), 0, GL_STATIC_DRAW_ARB);

        glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0,vectorsizeof<unsigned int>(shapes.at(i).mesh.indices), &(shapes.at(i).mesh.indices[0]));

        try
        {
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0,vectorsizeof<GLfloat>(shapes.at(i).mesh.positions), &(shapes.at(i).mesh.positions.at(0)));
        }
        catch (const std::exception &e)
        {
            cerr << e.what() << " during  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB" << endl;
            throw e;
        }

        // copy vertices starting from 0 offest
        // copy normals after vertices with the offset of mesh positions
        if (! shapes.at(i).mesh.normals.empty() )
        {
            try
            {
                glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vectorsizeof<GLfloat>(shapes.at(i).mesh.positions), vectorsizeof<GLfloat>(shapes.at(i).mesh.normals), &(shapes.at(i).mesh.normals.at(0)));
            }
            catch (const std::exception &e)
            {
                cerr << e.what() << " during  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB" << endl;
                throw e;
            }
        }
        else
        {
            cerr << "[ObjLoader2 LOG] No normals loaded" << endl;
        }
        // A MEMORY LEAK IS POSSIBLE HERE!!! XXX
        vertexBufferObjectIDs[i]= vertexBufferObjectId;
        indexBufferObjectIDs[i]=indexBufferObjectId;
#ifdef _DEBUG
        getGLerrors();
#endif
    }
    this->areGLBuffersInitialized=true;
}

/**
 * @brief ObjLoader2::normalizeToUnitBoundingBox It normalizes the size of the
 */
void ObjLoader2::normalizeToUnitBoundingBox()
{
    GLfloat barycenter[]={0.0f,0.0f,0.0f};
    GLfloat maxCoordShapes = std::numeric_limits<GLfloat>::min();
    GLfloat minCoordShapes = std::numeric_limits<GLfloat>::max();
    for (unsigned int i=0; i<shapes.size(); i++)
    {
        unsigned int size = shapes.at(i).mesh.positions.size();
        GLfloat maxCoord = *(std::max_element(shapes.at(i).mesh.positions.begin(),shapes.at(i).mesh.positions.end()));
        GLfloat minCoord = *(std::min_element(shapes.at(i).mesh.positions.begin(),shapes.at(i).mesh.positions.end()));

        maxCoordShapes = std::max(maxCoordShapes,maxCoord);
        minCoordShapes = std::min(minCoordShapes,minCoord);
        for (unsigned int k=0; k<size; k+=3)
        {
            for (int n=0; n<3;n++)
                barycenter[n]+=shapes[i].mesh.positions[k+n];
        }
    }

    for (unsigned int i=0; i<shapes.size();i++)
    {
        unsigned int size = shapes.at(i).mesh.positions.size();
        for (int n=0; n<3;n++)
            barycenter[n]/=(size/3.0f);
    }

    GLfloat d = maxCoordShapes-minCoordShapes;
    for (unsigned int i=0; i<shapes.size();i++)
    {
        unsigned int size = shapes.at(i).mesh.positions.size();
        for ( unsigned int k=0; k<size; k+=3 )
        {
            for (int n=0; n<3;n++)
            {
                shapes[i].mesh.positions[k+n]-=barycenter[n];
                shapes[i].mesh.positions[k+n]/=(d);
            }
        }
    }
}

/**
 * @brief ObjLoader2::center
 */
void ObjLoader2::center()
{

    GLfloat xmax = std::numeric_limits<GLfloat>::min(), ymax=std::numeric_limits<GLfloat>::min(),zmax=std::numeric_limits<GLfloat>::min();
    GLfloat xmin = std::numeric_limits<GLfloat>::max(), ymin=std::numeric_limits<GLfloat>::max(),zmin=std::numeric_limits<GLfloat>::max();

    for ( unsigned int i=0; i<shapes.size(); i++ )
    {
        xmax = std::max(xmax,*util::max_element_nth(shapes.at(i).mesh.positions.begin(),shapes.at(i).mesh.positions.end(), 3));
        ymax = std::max(ymax,*util::max_element_nth(shapes.at(i).mesh.positions.begin()+1,shapes.at(i).mesh.positions.end(), 3));
        zmax = std::max(zmax,*util::max_element_nth(shapes.at(i).mesh.positions.begin()+2,shapes.at(i).mesh.positions.end(), 3));

        xmin= std::min(xmin,*util::min_element_nth(shapes.at(i).mesh.positions.begin(),shapes.at(i).mesh.positions.end(), 3));
        ymin = std::min(ymin,*util::min_element_nth(shapes.at(i).mesh.positions.begin()+1,shapes.at(i).mesh.positions.end(), 3));
        zmin = std::min(zmin,*util::min_element_nth(shapes.at(i).mesh.positions.begin()+2,shapes.at(i).mesh.positions.end(), 3));
    }

    cerr << xmax << "," << xmin << "," << ymax << "," << ymin << "," << zmax << "," << zmin << endl;

    for ( unsigned int i=0; i<shapes.size(); i++ )
    {
        unsigned int size = shapes.at(i).mesh.positions.size();
        for ( unsigned int k=0; k<size; k+=3 )
        {
            shapes.at(i).mesh.positions.at(k)-=(xmax-xmin)/2;
            shapes.at(i).mesh.positions.at(k+1)-=(ymax-ymin)/2;
            shapes.at(i).mesh.positions.at(k+2)-=(zmax-zmin)/2;
        }
    }
}

/**
 * @brief ObjLoader2::drawShapes
 */
void ObjLoader2::drawShapes()
{
    if (selectedShapes.empty())
    {
        for (unsigned int i=0; i<shapes.size(); i++)
        {
            // Bind the vertex buffer for the current shape
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferObjectIDs[i]);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferObjectIDs[i]);
            // enable vertex arrays
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glNormalPointer(GL_FLOAT, 0, (void*)vectorsizeof(shapes.at(i).mesh.normals));
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glDrawElements(GL_TRIANGLES,vectorsizeof<unsigned int>(shapes.at(i).mesh.indices),GL_UNSIGNED_INT,0);
            glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
            glDisableClientState(GL_NORMAL_ARRAY);
            // bind with 0, so, switch back to normal pointer operation
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }
    }
    else
    {
        for (unsigned int i=0; i<selectedShapes.size(); i++)
        {
            // Bind the vertex buffer for the current shape
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferObjectIDs[i]);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferObjectIDs[i]);
            // enable vertex arrays
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glNormalPointer(GL_FLOAT, 0, (void*)vectorsizeof(selectedShapes.at(i).mesh.normals));
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glDrawElements(GL_TRIANGLES,vectorsizeof<unsigned int>(selectedShapes.at(i).mesh.indices),GL_UNSIGNED_INT,0);
            glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
            glDisableClientState(GL_NORMAL_ARRAY);
            // bind with 0, so, switch back to normal pointer operation
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        }
    }
}

/**
 * @brief ObjLoader2::getShapes
 * @return
 */
const std::vector<tinyobj::shape_t> &ObjLoader2::getShapes() const
{
    return this->shapes;
}

/**
 * @brief ObjLoader2::selectShapes
 * @param selectedShapesIndices
 * Select the shapes to draw
 */
void ObjLoader2::selectShapes(const std::vector<tinyobj::shape_t> &shapesToSelect)
{
    this->selectedShapes = shapesToSelect;
}


/**
 * @brief ObjLoader2::draw
 * @param drawMode
 */
void ObjLoader2::draw(GLenum drawMode)
{
    if (!areGLBuffersInitialized)
        throw std::runtime_error("OpenGL buffers are not initialized with the data. Instance a GL context and call ObjLoader2::initializeBuffers() first");

    if (this->vertexBufferObjectIDs.empty() )
        throw std::runtime_error("No buffers allocated, call initializeBuffers() first one the GL context has been initialized");
    // https://developer.apple.com/library/mac/#documentation/graphicsimaging/conceptual/opengl-macprogguide/opengl_vertexdata/opengl_vertexdata.html
    if (drawMode!=GL_FILL && drawMode!=GL_LINE && drawMode!=GL_POINT && drawMode!= (GL_LINE | GL_FILL) )
        throw std::runtime_error("Error in draw mode: Can only be GL_POINT or GL_LINE or GL_FILL or GL_LINE | GL_FILL");

    glColor4fv(fillColor.data());
    glPolygonMode(GL_FRONT_AND_BACK,drawMode);
    drawShapes();
    getGLerrors();
}

/**
 * @brief ObjLoader2::getObjModel
 * @return
 */
const std::vector<tinyobj::shape_t> &ObjLoader2::getObjModel() const
{
    return this->shapes;
}

/**
 * @brief ObjLoader2::getInfo
 */
void ObjLoader2::getInfo(std::ostream &os)
{

    os << "[ObjLoader2 LOG] #Shapes: " << shapes.size() << endl;
    for (unsigned int i=0; i<shapes.size(); i++)
    {
        os << "[ObjLoader2 LOG]\tShape[" << i << "] Name: "  << shapes[i].name << endl;
        os << "[ObjLoader2 LOG]\tShape[" << i << "] NumVertices: "  << shapes[i].mesh.positions.size()/3 << endl;
        os << "[ObjLoader2 LOG]\tShape[" << i << "].NumNormals: "  << shapes[i].mesh.normals.size()/3 << endl;
        os << "[ObjLoader2 LOG]\tShape[" << i << "].NumTexcoords: "  << shapes[i].mesh.texcoords.size()/2 << endl;
        os << "[ObjLoader2 LOG]\tShape[" << i << "].NumIndices: "  << shapes[i].mesh.indices.size() << endl;
    }
}

/**
 * @brief ObjLoader2::getInfo Print the output to standard error if not specified
 */
void ObjLoader2::getInfo()
{
    this->getInfo(cerr);
}


/**
 * @brief ObjLoader2::setLineColor
 * @param lineRed
 * @param lineGreen
 * @param lineBlue
 * @param lineAlpha
 */
void ObjLoader2::setLineColor(GLfloat lineRed, GLfloat lineGreen, GLfloat lineBlue, GLfloat lineAlpha)
{
    lineColor <<  lineRed,  lineGreen,  lineBlue,  lineAlpha;
}

/**
 * @brief ObjLoader2::setFillColor
 * @param fillRed
 * @param fillGreen
 * @param fillBlue
 * @param fillAlpha
 */
void ObjLoader2::setFillColor(GLfloat fillRed, GLfloat fillGreen, GLfloat fillBlue, GLfloat fillAlpha)
{
    fillColor <<  fillRed,  fillGreen,  fillBlue,  fillAlpha;
}

/**
 * @brief ObjLoader2::mapToPoints
 * @param shapeIndex
 * @return
 */
/*
const GLVertices & ObjLoader2::mapToPoints(int shapeIndex)
{
    return Eigen::Map<GLVertices >(&shapes.at(shapeIndex).mesh.positions.at(0), 3, shapes.at(shapeIndex).mesh.positions.size() );
}
*/
