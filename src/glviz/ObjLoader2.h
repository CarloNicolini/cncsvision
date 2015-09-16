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

#ifndef _OBJLOADER2_H_
#define _OBJLOADER2_H_
#include <Eigen/Core>
#include <vector>
#include <string>
#include <map>
#include "IncludeGL.h"
#include "TinyObjLoader.h"

template<typename T>
size_t vectorsizeof(const typename std::vector<T>& vec)
{
    return sizeof(T) * vec.size();
}

typedef  Eigen::Matrix< GLfloat, 3,Eigen::Dynamic,Eigen::ColMajor> GLVertices;

// http://openglbook.com/the-book/chapter-3-index-buffer-objects-and-primitive-types/
class ObjLoader2
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    ObjLoader2();
    ObjLoader2(const std::string &filename);
    ~ObjLoader2();
    bool load(const std::string &filename, const std::string &pathname="");
    void normalizeToUnitBoundingBox();
    void center();
    void initializeBuffers();
    void setLineColor(GLfloat lineRed, GLfloat lineGreen, GLfloat lineBlue, GLfloat lineAlpha);
    void setFillColor(GLfloat fillRed, GLfloat fillGreen, GLfloat fillBlue, GLfloat fillAlpha);
    void draw(GLenum drawMode=GL_FILL);
    void drawSelected(GLenum drawMode=GL_FILL);
    void selectShapes(const std::vector<tinyobj::shape_t> &selectedShapes);
    const std::vector<tinyobj::shape_t> &getObjModel() const;
    const std::vector<tinyobj::shape_t> &getShapes() const;
    void getInfo(std::ostream &os);
    void getInfo();
    //const GLVertices & mapToPoints(int shapeIndex);
private:
    void drawShapes();
    std::map<int,GLuint> vertexBufferObjectIDs;
    std::map<int,GLuint> indexBufferObjectIDs;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::shape_t> selectedShapes;
    bool areGLBuffersInitialized;
    Eigen::Matrix<GLfloat,4,1> fillColor;
    Eigen::Matrix<GLfloat,4,1> lineColor;

};

#endif
