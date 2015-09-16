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

#include "VolumetricMeshIntersection.h"

/**
 * @brief VolumetricMeshIntersection::VolumetricMeshIntersection
 */
VolumetricMeshIntersection::VolumetricMeshIntersection()
{
    obj=NULL;
}

/**
 * @brief VolumetricMeshIntersection::VolumetricMeshIntersection
 * @param textureSizeX
 * @param textureSizeY
 * @param textureSizeZ
 */
VolumetricMeshIntersection::VolumetricMeshIntersection(unsigned int textureSizeX, unsigned int textureSizeY, unsigned int textureSizeZ)
{
    VolumetricSurfaceIntersection::resize(textureSizeX,textureSizeY,textureSizeZ);
    obj=NULL;
}

/**
 * @brief VolumetricMeshIntersection::~VolumetricMeshIntersection
 */
VolumetricMeshIntersection::~VolumetricMeshIntersection()
{
    if (obj!=NULL)
        delete obj;
    if (meshStruct.shader)
        delete meshStruct.shader;
}

void VolumetricMeshIntersection::initializeTexture()
{
    if (this->volume3DTextureID)
        glDeleteTextures(1,&(this->volume3DTextureID));

    glGenTextures(1, &(this->volume3DTextureID));

    glBindTexture(GL_TEXTURE_3D, this->volume3DTextureID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    // Important to enable bilinear filtering and smoothing
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // This disables bilinear filtering
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, this->textureSizeX, this->textureSizeY, this->textureSizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,(GLvoid *) &(texture3DVolume.at(0)));
    // We can clean the volume after it is loaded on GPU
    texture3DVolume.clear();

    getGLerrors();
}

/**
 * @brief VolumetricMeshIntersection::initializeSurfaceShaders
*/
void VolumetricMeshIntersection::initializeSurfaceShaders()
{
    glsl::glShaderManager SM;

    const GLcharARB vertexShader[] = STATIC_STRINGIFY(
    uniform float step;
    varying vec3 texture_coordinate;
    uniform float objSize;
    uniform vec3 objOffset;
    void main()
    {
        vec4 v=gl_Vertex;
        // This is to rotated the object
        v.xz = vec2(cos(step)*v.x+sin(step)*v.z,-sin(step)*v.x+cos(step)*v.z);
        // Compute the z position given x and y on a circular domain of radius 1 (diameter 2)
        //texture_coordinate= vec3(((v.xz/radius+1.0)*0.5),(v.y-objOffset.y)/objSize);
        texture_coordinate= vec3(   ((v.xz-objOffset.xz)/objSize+1.0)*0.5,    ((v.y-objOffset.y)/objSize+1.0)*0.5);
        gl_Position = gl_ModelViewProjectionMatrix*v;
    }
    );

    const GLcharARB fragmentShader[] = STATIC_STRINGIFY(
    varying vec3 texture_coordinate;
    uniform sampler3D my_color_texture;
    uniform vec4 uniformColor;
    varying vec4 pvertex;
    void main()
    {
        //vec4 uniformColor = vec4(1.0,1.0,0.0,1.0);
        if ( texture_coordinate.x <=0.0 || texture_coordinate.x >= 1.0 || texture_coordinate.z <= 0.0 || texture_coordinate.z >= 1.0)
        {
            gl_FragColor =vec4(0.0,0.0,0.0,1.0); //Can be uniformColor to color again the thing
        }
        else
        {
            gl_FragColor = uniformColor*texture3D(my_color_texture, texture_coordinate);
        }
    }
    );
    meshStruct.shader= SM.loadfromMemory(vertexShader,fragmentShader);
    meshStruct.useParametricSurfaceFiltering=false;
}

/**
 * @brief VolumetricMeshIntersection::initializeSurfaceShaders
 * @param vertexShader
 * @param fragmentShader
 */
void VolumetricMeshIntersection::initializeSurfaceShaders(const GLcharARB *vertexShader,const GLcharARB *fragmentShader)
{
    glsl::glShaderManager SM;
    meshStruct.shader = SM.loadfromMemory(vertexShader,fragmentShader);
    meshStruct.useParametricSurfaceFiltering=true;
}

/**
 * @brief VolumetricMeshIntersection::setObj
 * @param _obj
 */
void VolumetricMeshIntersection::setObj(const ObjLoader2 *_obj)
{
    if (!obj)
        this->obj = const_cast<ObjLoader2*>(_obj);
    else
        throw std::runtime_error("Assigning non valid NULL pointer as mesh object");
}

/**
 * @brief VolumetricMeshIntersection::loadObj
 * @param filename
 */
void VolumetricMeshIntersection::loadObj(const string &filename)
{
    if (obj==NULL)
    {
        obj = new ObjLoader2();
        obj->load(filename);
        obj->initializeBuffers();
        obj->getInfo();
    }
}

/**
 * @brief VolumetricMeshIntersection::draw
 */
void VolumetricMeshIntersection::draw()
{
    glEnable(GL_TEXTURE_3D);
    meshStruct.shader->begin();
    meshStruct.shader->setUniform4f(const_cast<GLcharARB*>("uniformColor"),uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
    meshStruct.shader->setUniform1f(const_cast<GLcharARB*>("step"),meshStruct.rotationAngle);
    meshStruct.shader->setUniform3f(const_cast<GLcharARB*>("objOffset"),meshStruct.offsetX,meshStruct.offsetY,meshStruct.offsetZ);
    meshStruct.shader->setUniform1f(const_cast<GLcharARB*>("objSize"),meshStruct.radius);
    if (meshStruct.useParametricSurfaceFiltering)
        meshStruct.shader->setUniform1f("thickness",meshStruct.thickness);
    //if (obj==NULL)
    // throw std::runtime_error("Must load an OBJ file before drawing it. Call loadObj");
    glPushMatrix();
    glTranslated(meshStruct.x,meshStruct.y,meshStruct.z);
    obj->draw();
    glPopMatrix();
    meshStruct.shader->end();
    glDisable(GL_TEXTURE_3D);

    if ( meshStruct.showMesh )
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glLineWidth(0.1f);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        // Draw the helicoid 3D texture volume
        glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslated(meshStruct.offsetX,meshStruct.offsetY,meshStruct.offsetZ);
        glutWireCube(meshStruct.radius*2);
        glPopMatrix();

        glPushMatrix();
        glTranslated(meshStruct.x,meshStruct.y,meshStruct.z);
        //glRotated(meshStruct.rotationAngle*180.0/M_PI,0,1,0);
        obj->draw(GL_LINE);
        glPopMatrix();
        glPopAttrib();
    }
}

