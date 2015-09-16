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

#include "VolumetricSurfaceIntersection.h"
// Needed for fast generation of grid of points as domain for the surfaces
#include "Grid.h"
// Needed for fast generation of an Icosphere, triangulated sphere

// The constructor allocates the memory once, which is the heaviest operation
/**
 * @brief VolumetricSurfaceIntersection::VolumetricSurfaceIntersection
 * @param _sizeX
 * @param _sizeY
 * @param _sizeZ
 */
VolumetricSurfaceIntersection::VolumetricSurfaceIntersection(unsigned int _sizeX, unsigned int _sizeY, unsigned int _sizeZ) : textureSizeX(_sizeX),textureSizeY(_sizeY),textureSizeZ(_sizeZ),volume3DTextureID(0),uniformColor(NULL),currentSurface(-1)
{
    useRandomSpheresColor=false;
    texture3DfillValue=0;
    this->uniformColor = new GLfloat[4];

    for (int i=0; i<4;i++)
        this->uniformColor[i]=1.0;

    // http://pinyotae.blogspot.it/2009/06/note-on-boost-multidimensional-array.html
    texture3DVolume.resize(textureSizeX*textureSizeY*textureSizeZ);

    memset(&(texture3DVolume.at(0)),texture3DfillValue,sizeof(GLubyte)*textureSizeX*textureSizeY*textureSizeZ);

    parabolaSurface.shader=NULL;
    parabolicCylinderSurface.shader=NULL;
    cubeSurface.shader=NULL;
    ellipsoidSurface.shader=NULL;
    coneSurface.shader=NULL;
    ellipticCylinderSurface.shader=NULL;
    potatoSurface.shader = NULL;

    potatoSurface.sphere = new IcoSphere(ICOSPHERE_LEVEL);
}

/**
 * @brief VolumetricSurfaceIntersection::VolumetricSurfaceIntersection
 */
VolumetricSurfaceIntersection::VolumetricSurfaceIntersection() : volume3DTextureID(0), uniformColor(NULL), currentSurface(-1)
{
    useRandomSpheresColor=false;
    texture3DfillValue=0;
    this->uniformColor = new GLfloat[4];
    for (int i=0; i<4;i++)
        this->uniformColor[i]=1.0;

    this->volume3DTextureID=0;
    // Set the parabola surface things
    this->parabolaSurface.centerX=parabolaSurface.centerY=parabolaSurface.centerZ=0.0;
    this->parabolaSurface.curvature=1.0;

    this->parabolicCylinderSurface.centerX=this->parabolicCylinderSurface.centerY=this->parabolicCylinderSurface.centerZ=0.0;
    this->parabolicCylinderSurface.curvature=1.0;

    // Set the elliptic cylinder things
    this->ellipticCylinderSurface.axisX=1.0;
    this->ellipticCylinderSurface.height=1.0;
    this->ellipticCylinderSurface.axisZ=1.0;
    this->cubeSurface.edge=1.0;

    this->potatoSurface.seed = 1.0;
    this->potatoSurface.scale=1.0f;
    this->potatoSurface.normalScale = 0.25f;
    this->potatoSurface.sphere = new IcoSphere(ICOSPHERE_LEVEL);

    parabolaSurface.shader=NULL;
    cubeSurface.shader=NULL;
    ellipsoidSurface.shader=NULL;
    coneSurface.shader=NULL;
    ellipticCylinderSurface.shader=NULL;
    potatoSurface.shader=NULL;
}

/**
 * @brief VolumetricSurfaceIntersection::resize
 * @param sizeX
 * @param sizeY
 * @param sizeZ
 */
void VolumetricSurfaceIntersection::resize(unsigned int _sizeX, unsigned int _sizeY, unsigned int _sizeZ)
{
    this->textureSizeX = _sizeX;
    this->textureSizeY = _sizeY;
    this->textureSizeZ = _sizeZ;
    texture3DVolume.clear();
    texture3DVolume.resize(textureSizeX*textureSizeY*textureSizeZ);
    memset(&(texture3DVolume.at(0)),texture3DfillValue,sizeof(GLubyte)*textureSizeX*textureSizeY*textureSizeZ);
#ifdef USE_OBJ
    obj = new ObjLoader2();
    obj->load("../data/objmodels/helicoid.obj");
    obj->initializeBuffers();
    obj->getInfo();
#endif
}
/**
 * @brief VolumetricSurfaceIntersection::~VolumetricSurfaceIntersection
 */
VolumetricSurfaceIntersection::~VolumetricSurfaceIntersection()
{
    if (this->volume3DTextureID)
        glDeleteTextures(1,&(this->volume3DTextureID));
    //    if (texture3DVolume)
    //        delete texture3DVolume;
    if (uniformColor)
        delete[] uniformColor;

    switch ( currentSurface )
    {
    case SurfaceParaboloid:
    {
        if (parabolaSurface.shader)
            delete parabolaSurface.shader ;
        break;
    }
    case SurfaceParabolicCylinder:
    {
        if (parabolicCylinderSurface.shader)
            delete parabolicCylinderSurface.shader ;
        break;
    }
    case SurfaceCone:
    {
        if (coneSurface.shader)
            delete coneSurface.shader ;
        break;
    }
    case SurfaceEllipsoid:
    {
        if (ellipsoidSurface.shader)
            delete ellipsoidSurface.shader ;
        break;
    }
    case SurfaceEllipticCylinder:
    {
        if (ellipticCylinderSurface.shader)
            delete ellipticCylinderSurface.shader ;
        break;
    }
    case SurfaceCube:
    {
        if (cubeSurface.shader)
            delete cubeSurface.shader;
        break;
    }
    case SurfacePotato:
    {
        if (potatoSurface.shader)
            delete potatoSurface.shader;
        if (potatoSurface.sphere)
            delete potatoSurface.sphere;
        break;
    }
    }

}

int VolumetricSurfaceIntersection::sub2ind(int x, int y, int z)
{
    // http://nadeausoftware.com/articles/2012/06/c_c_tip_how_loop_through_multi_dimensional_arrays_quickly#Method2Nestedloopswithlineararrayandindexeswith3multiplies
    //return x * textureSizeY* textureSizeZ+ y * textureSizeZ+ z;
    return  textureSizeY* textureSizeX* z + textureSizeX* y + x;
}

/**
* @brief  VolumetricSurfaceIntersection::setUniformColor
* @param color
**/
void VolumetricSurfaceIntersection::setUniformColor(const GLfloat color[4])
{
    if (!uniformColor)
    {
        uniformColor = new GLfloat[4];
    }
    for (int i=0; i<4;i++)
        this->uniformColor[i]=color[i];
}

/**
 * @brief VolumetricSurfaceIntersection::fillVolumeWithSpheres
 * @param nSpheres
 * @param minRadius
 * @param maxRadius
 */
void VolumetricSurfaceIntersection::fillVolumeWithSpheres( int nSpheres, int minRadius, int maxRadius)
{
    this->nSpheres = nSpheres;
    this->sphereRadiusMin = minRadius;
    this->sphereRadiusMax = maxRadius;
    //    Timer timer;
    //    timer.start();
    if ( texture3DVolume.empty() )
        this->resize(this->textureSizeX,this->textureSizeY,this->textureSizeZ);
    memset(&(texture3DVolume.at(0)),texture3DfillValue,sizeof(GLubyte)*textureSizeX*textureSizeY*textureSizeZ);
    // This is a bogus algorithm to avoid intersecting spheres, it can be done better by means
    // of intersection graph and maximal independent set but it need a lot of more code!!
    spheres.clear();
    int radius = mathcommon::unifRand(minRadius,maxRadius);
    spheres[0]=Circle3D<int>( mathcommon::unifRand(radius,textureSizeX-radius), mathcommon::unifRand(radius,textureSizeY-radius), mathcommon::unifRand(radius,textureSizeZ-radius),radius);

    int noncoll=0;
    double totalVolume = this->textureSizeX*this->textureSizeY*this->textureSizeZ;
    double spheresVolume=0.0;

    while ( noncoll<nSpheres )
    {
        int radius = mathcommon::unifRand(minRadius,maxRadius);
        int centerx = mathcommon::unifRand(radius,textureSizeX-radius);
        int centery = mathcommon::unifRand(radius,textureSizeY-radius);
        int centerz = mathcommon::unifRand(radius,textureSizeZ-radius);
        Circle3D<int> curSphere(centerx,centery,centerz,radius);
        bool tmpIsCollidingWithSomeOtherElement=false;
        for (map<int, Circle3D<int> >::iterator iter = spheres.begin(); iter!=spheres.end(); ++iter)
        {
            if  ( iter->second.checkCollision(curSphere,2) )
            {
                tmpIsCollidingWithSomeOtherElement=true;
            }
        }
        if ( !tmpIsCollidingWithSomeOtherElement )
        {
            //cout << spheresVolume << "\t" << spheresVolume/totalVolume*100.0 <<  "\t" << noncoll << endl;
            spheresVolume+=4.0/3.0*M_PI*radius*radius*radius;
            spheres[noncoll++]=curSphere;
        }
        if ( spheresVolume/totalVolume > 0.35 ) // http://en.wikipedia.org/wiki/Random_close_pack
        {
            maxRadius-=1;
            if (maxRadius < minRadius)
            {
                cerr << "[VolumetricSurfaceIntersection] Stop adding spheres because total volume is too much, total spheres = " << spheres.size() << endl;
                break;
            }
        }
    }

    /*
    spheres.clear();
    int gridSpacing = 16;
    int k=0;
    for (int x=gridSpacing;x<textureSizeX-gridSpacing; x+=gridSpacing)
    {
        for (int y=gridSpacing; y<textureSizeY-gridSpacing;y+=gridSpacing)
        {
            for (int z=gridSpacing; z<textureSizeZ-gridSpacing; z+=gridSpacing)
            {
                spheres.insert(std::pair<int,Circle3D<int> > (k++,Circle3D<int>(x,y,z,1 )));
            }
        }
    }

    radius = 8;
    for (int i=0; i<spheres.size(); i++)
    {
        spheres[i].radius = mathcommon::unifRand(radius,radius);
        spheres[i].centerx += mathcommon::unifRand(-textureSizeX/64,textureSizeX/64);
        spheres[i].centery += mathcommon::unifRand(-textureSizeX/64,textureSizeX/64);
        spheres[i].centerz += mathcommon::unifRand(-textureSizeX/64,textureSizeX/64);
    }
*/
    writeSpheresToTexture(255);
    spheres.clear();
    //    cerr << "Sphere filling time elapsed " << timer.getElapsedTimeInMilliSec() << " [ms]" << endl;
}

/**
 * @brief VolumetricSurfaceIntersection::fillVolumeWithRandomDots
 * @param nRandomDots
 * @param size
 */
void VolumetricSurfaceIntersection::fillVolumeWithRandomDots(int nRandomDots, int radius)
{
    if ( texture3DVolume.empty() )
        this->resize(this->textureSizeX,this->textureSizeY,this->textureSizeZ);
    memset(&(texture3DVolume.at(0)),texture3DfillValue,sizeof(GLubyte)*textureSizeX*textureSizeY*textureSizeZ);
    for (int i=0; i<nRandomDots;++i)
    {
        int centerx = mathcommon::unifRand(radius,textureSizeX-radius);
        int centery = mathcommon::unifRand(radius,textureSizeY-radius);
        int centerz = mathcommon::unifRand(radius,textureSizeZ-radius);

        for (int z=centerz-radius; z<centerz+radius;++z)
        {

            int tYtXx  = textureSizeY* textureSizeX* z;
            for ( int y=centery-radius; y<centery+radius;++y)
            {
                int tXY = textureSizeX* y;
                for ( int x= centerx-radius; x<centerx+radius; ++x)
                {
                    texture3DVolume[tYtXx+tXY + x] = 255;
                }
            }
        }
    }
}

/**
 * @brief VolumetricSurfaceIntersection::writeSpheresToTexture
 * @param value
 */
void VolumetricSurfaceIntersection::writeSpheresToTexture(int value)
{
    for (unsigned int i=0; i<spheres.size(); i++)
    {
        int sphereGrayColor =value;
        if (useRandomSpheresColor)
        {
            if (rand()%2)
                sphereGrayColor = mathcommon::unifRand(0,117);
            else
                sphereGrayColor=mathcommon::unifRand(137,255);
        }

        int radius= spheres[i].radius;
        int centerx = spheres[i].centerx;
        int centery = spheres[i].centery;
        int centerz = spheres[i].centerz;

        int cxmin=centerx-radius;
        int cxmax=centerx+radius;
        int cymin=centery-radius;
        int cymax=centery+radius;
        int czmin = centerz-radius;
        int czmax = centerz+radius;

        int radius2 = SQR(radius);
        for (int z=czmin; z<czmax;++z)
        {
            int z2 = SQR((z-centerz));
            int tYtXx  = textureSizeY* textureSizeX* z;
            for ( int y=cymin; y<cymax;++y)
            {
                int z2y2=z2+SQR(y-centery);
                int tXY = textureSizeX* y;
                for ( int x= cxmin; x<cxmax; ++x)
                {
                    if ( z2y2+SQR(x-centerx) < radius2)
                        texture3DVolume[tYtXx+tXY + x] = sphereGrayColor;
                }
            }
        }
    }
}

/**
 * @brief VolumetricSurfaceIntersection::updateTexture
 */
void VolumetricSurfaceIntersection::initializeTexture()
{
    getGLerrors();
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
    getGLerrors();
    // This disables bilinear filtering
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, this->textureSizeX, this->textureSizeY, this->textureSizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,(GLvoid *) &(texture3DVolume.at(0)));
    // We can clean the volume after it is loaded on GPU
    texture3DVolume.clear();

}

/**
 * @brief VolumetricSurfaceIntersection::initializeSurfaceShaders
 * @param surface
 */
void VolumetricSurfaceIntersection::initializeSurfaceShaders(int surface)
{
    if (surface < SurfaceParaboloid || surface > SurfacePotato )
        throw std::out_of_range("Surface not supported, values are in [0,6]");

    this->currentSurface=surface;

    switch ( surface )
    {
    case SurfaceParaboloid:
    {
        if (parabolaSurface.shader)
            delete parabolaSurface.shader ;
        parabolaSurface.shader = new glsl::glShader;
        break;
    }
    case SurfaceParabolicCylinder:
    {
        if (parabolicCylinderSurface.shader)
            delete parabolicCylinderSurface.shader ;
        parabolicCylinderSurface.shader = new glsl::glShader;
        break;
    }
    case SurfaceCone:
    {
        if (coneSurface.shader)
            delete coneSurface.shader ;
        coneSurface.shader = new glsl::glShader;
        break;
    }
    case SurfaceEllipsoid:
    {
        if (ellipsoidSurface.shader)
            delete ellipsoidSurface.shader ;
        ellipsoidSurface.shader = new glsl::glShader;
        break;
    }
    case SurfaceEllipticCylinder:
    {
        if (ellipticCylinderSurface.shader)
            delete ellipticCylinderSurface.shader ;
        ellipticCylinderSurface.shader = new glsl::glShader;
        break;
    }
    case SurfaceCube:
    {
        if (cubeSurface.shader)
            delete cubeSurface.shader;
        cubeSurface.shader = new glsl::glShader;
        break;
    }
    case SurfacePotato:
    {
        if (potatoSurface.shader)
            delete potatoSurface.shader;
        potatoSurface.shader = new glsl::glShader;
        break;
    }
    }

    glsl::glShaderManager SM;
    // Fragment shader is the same for every surface
    const GLcharARB fragmentShader[] =
            STATIC_STRINGIFY(
                const vec3 X = vec3(1.0,0.0,0.0);
                const vec3 Y = vec3(0.0,1.0,0.0);
                const vec3 Z = vec3(0.0,0.0,1.0);
                varying vec3 texture_coordinate;
            uniform sampler3D my_color_texture;
    uniform vec4 uniformColor;
    void main()
    {
        vec4 uniformColor = vec4(1.0,1.0,1.0,1.0);
        vec4 finalColor;
        //finalColor = (texture3D(my_color_texture, texture_coordinate+X)+texture3D(my_color_texture-X, texture_coordinate))*0.5
        //        +(texture3D(my_color_texture, texture_coordinate+Y)+texture3D(my_color_texture-Y, texture_coordinate))*0.5
        //        +(texture3D(my_color_texture, texture_coordinate+Z)+texture3D(my_color_texture-Z, texture_coordinate))*0.5;
        if ( texture_coordinate.x <=0.0 || texture_coordinate.x >= 1.0 || texture_coordinate.z <= 0.0 || texture_coordinate.z >= 1.0 )
            gl_FragColor =vec4(0.0,0.0,0.0,1.0); //Can be uniformColor to color again the thing
        else
            gl_FragColor = uniformColor*texture3D(my_color_texture, texture_coordinate);
    }
    );

    // Set the correct vertex shader
    switch ( surface )
    {
    case SurfaceParaboloid:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float curvature;
        uniform float zToXYTextureRatio;
        void main()
        {
            vec4 v=gl_Vertex;
            v.z = curvature*(v.x*v.x+v.y*v.y)-curvature;
            v.z = -v.z;
            texture_coordinate = vec3(((v.xy+1.0)*0.5),(v.z)*zToXYTextureRatio);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        parabolaSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfaceParabolicCylinder:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float curvature;
        uniform float zToXYTextureRatio;
        void main()
        {
            vec4 v=gl_Vertex;
            v.z = curvature*(v.x*v.x)-curvature;
            v.z = -v.z;
            texture_coordinate = vec3(((v.xy+1.0)*0.5),v.z*zToXYTextureRatio);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        parabolicCylinderSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfaceCone:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float c;
        uniform float zToXYTextureRatio;
        void main()
        {
            vec4 v=gl_Vertex;
            v.z = -c*sqrt(v.x*v.x+v.y*v.y);
            texture_coordinate = vec3((v.x+1.0)*0.5,(v.y+1.0)*0.5,(v.z+curvature)*0.5*zToXYTextureRatio);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        coneSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfaceEllipsoid:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float axisX;
        uniform float axisY;
        uniform float axisZ;
        void main()
        {
            vec4 v=gl_Vertex;
            float den = v.x*v.x/(axisX*axisX)+v.y*v.y/(axisY*axisY);
            v.z = sqrt( axisZ*axisZ*(1.0- den) );
            texture_coordinate = vec3(v.x+0.5,v.y+0.5,v.z+0.5);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        ellipsoidSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfaceEllipticCylinder:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float axisX;
        uniform float height;
        uniform float axisZ;
        void main()
        {
            vec4 v=gl_Vertex;
            texture_coordinate = v.xyz+vec3(0.5);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        ellipticCylinderSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfaceCube:
    {
        const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                    varying vec3 texture_coordinate;
                uniform float axisX;
        uniform float height;
        uniform float axisZ;
        uniform float zToXYTextureRatio;
        void main()
        {
            vec4 v=gl_Vertex;
            texture_coordinate = vec3((v.xy+1.0)*0.5,((v.z)+1.0)*0.5*zToXYTextureRatio);
            gl_Position = gl_ModelViewProjectionMatrix*v;
        }
        );
        cubeSurface.shader = SM.loadfromMemory(vertexShader,fragmentShader);
        break;
    }
    case SurfacePotato:
    {

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        // Important to enable bilinear filtering and smoothing
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifdef __linux__
        char vertexShaderFile[] = "../experiments/texturing/potato_perlin_deformed.vert";
        char fragmentShaderFile[] = "../experiments/texturing/potato_perlin_deformed.frag";
#endif
        
#ifdef __APPLE__
        char vertexShaderFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/potato_perlin_deformed.vert";
        char fragmentShaderFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/potato_perlin_deformed.frag";
#endif

#ifdef _WIN32
        char vertexShaderFile[] = "../../experiments/texturing/potato_perlin_deformed.vert";
        char fragmentShaderFile[] = "../../experiments/texturing/potato_perlin_deformed.frag";
#endif

        potatoSurface.shader = SM.loadfromFile(vertexShaderFile,fragmentShaderFile);
        break;
    }
    }
}

/**
 * @brief VolumetricSurfaceIntersection::draw
 * @param surface
 */
void VolumetricSurfaceIntersection::draw()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_TEXTURE_3D);

    glBindTexture(GL_TEXTURE_3D, this->volume3DTextureID);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);

    switch ( this->currentSurface )
    {
    case SurfaceParaboloid:
    {
        parabolaSurface.shader->begin();
        parabolaSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        parabolaSurface.shader->setUniform1f((GLcharARB*)"curvature",parabolaSurface.curvature);
        parabolaSurface.shader->setUniform1f((GLcharARB*)"zToXYTextureRatio",(float)textureSizeX/(float)textureSizeZ);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad,GLU_FILL);
        gluDisk(quad,0.0,1.0,100,100);
        gluDeleteQuadric(quad);

        parabolaSurface.shader->end();
        break;
    }
    case SurfaceParabolicCylinder:
    {
        parabolicCylinderSurface.shader->begin();
        parabolicCylinderSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        parabolicCylinderSurface.shader->setUniform1f((GLcharARB*)"curvature",parabolicCylinderSurface.curvature);
        parabolicCylinderSurface.shader->setUniform1f((GLcharARB*)"zToXYTextureRatio",(float)textureSizeX/(float)textureSizeZ);
        // Prepares the domain where the cylinder with parabolic section will be drawn
        Grid grid;
        grid.setRowsAndCols(100,100);
        grid.init(-1,1,-1,1);
        bool fillGrid=true;
        grid.draw(fillGrid);
        parabolicCylinderSurface.shader->end();
        break;
    }
    case SurfaceEllipsoid:
    {
        ellipsoidSurface.shader->begin();
        ellipsoidSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        ellipsoidSurface.shader->setUniform1f((GLcharARB*)"axisX",ellipsoidSurface.axisX);
        ellipsoidSurface.shader->setUniform1f((GLcharARB*)"axisY",ellipsoidSurface.axisY);
        ellipsoidSurface.shader->setUniform1f((GLcharARB*)"axisZ",ellipsoidSurface.axisZ);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad,GLU_FILL);
        gluDisk(quad,0.0,1.0,100,100);
        gluDeleteQuadric(quad);
        ellipsoidSurface.shader->end();
        break;
    }
    case SurfaceCone:
    {
        coneSurface.shader->begin();
        coneSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        coneSurface.shader->setUniform1f((GLcharARB*)"c",coneSurface.c);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad,GLU_FILL);
        gluDisk(quad,0.0,1.0,100,100);
        gluDeleteQuadric(quad);
        coneSurface.shader->end();
        break;
    }
    case SurfaceEllipticCylinder:
    {
        ellipticCylinderSurface.shader->begin();
        ellipticCylinderSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        ellipticCylinderSurface.shader->setUniform1f((GLcharARB*)"axisX",ellipticCylinderSurface.axisX);
        ellipticCylinderSurface.shader->setUniform1f((GLcharARB*)"axisZ",ellipticCylinderSurface.axisZ);
        ellipticCylinderSurface.shader->setUniform1f((GLcharARB*)"height",ellipticCylinderSurface.height);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad,GLU_FILL);
        gluCylinder(quad,1.0,1.0,1.0,100,100);
        gluDeleteQuadric(quad);
        ellipticCylinderSurface.shader->end();
        break;
    }
    case SurfaceCube:
    {
        cubeSurface.shader->begin();
        cubeSurface.shader->setUniform4f((GLcharARB*)"uniformColor",uniformColor[0],uniformColor[1],uniformColor[2],uniformColor[3]);
        cubeSurface.shader->setUniform1f((GLcharARB*)"zToXYTextureRatio",(float)textureSizeX/(float)textureSizeZ);
        glPushMatrix();
        glutSolidCube(cubeSurface.edge);
        glPopMatrix();
        cubeSurface.shader->end();
        break;
    }
    case SurfacePotato:
    {
        glPushMatrix();
        Eigen::Affine3f MV; // ModelView matrix
        Eigen::Projective3f P; // Projection matrix
        Eigen::Matrix3f N; // Normal Matrix

        glGetFloatv(GL_MODELVIEW_MATRIX,MV.data()); // collect the modelview matrix
        glGetFloatv(GL_PROJECTION_MATRIX,P.data()); // collect the projection matrix
        Eigen::Projective3f MVP = P*MV; // ModelView Projection Matrix
        N = MV.linear().inverse().transpose();

        potatoSurface.shader->begin();
        potatoSurface.shader->setUniformMatrix4fv((GLcharARB*)"GL_ModelViewProjectionMatrix",1,GL_FALSE,MVP.data());
        potatoSurface.shader->setUniformMatrix4fv((GLcharARB*)"GL_ModelViewMatrix",1,GL_FALSE,MV.data());
        potatoSurface.shader->setUniformMatrix3fv((GLcharARB*)"GL_NormalMatrix",1,GL_FALSE,N.data());

        potatoSurface.shader->setUniform4fv((GLcharARB*)"GL_AmbientColor",1,potatoSurface.ambientColor.data());
        potatoSurface.shader->setUniform4fv((GLcharARB*)"GL_DiffuseColor",1,potatoSurface.diffuseColor.data());
        potatoSurface.shader->setUniform4fv((GLcharARB*)"GL_LightPosition",1,potatoSurface.lightPosition.data());

        //potatoSurface.shader->setUniform3fv((GLcharARB*)"GL_SpecularColor",1,potatoSurface.specularColor.data());
        //potatoSurface.shader->setUniform1f((GLcharARB*)"GL_Shininess",potatoSurface.shininess);

        potatoSurface.shader->setUniform1f((GLcharARB*)"vertexScale",potatoSurface.scale);
        potatoSurface.shader->setUniform1f((GLcharARB*)"normalScale",potatoSurface.normalScale);
        potatoSurface.shader->setUniform1f((GLcharARB*)"zScale",potatoSurface.zScale);
        potatoSurface.shader->setUniform1f((GLcharARB*)"seed",potatoSurface.seed);
        potatoSurface.sphere->draw(ICOSPHERE_LEVEL);
        potatoSurface.shader->end();
        glPopMatrix();
        break;
    }
    }
    glBindTexture(GL_TEXTURE_3D, 0);
    glPopAttrib();
}

/**
 * @brief VolumetricSurfaceIntersection::setTexture3DfillValue
 * @param val
 */
void VolumetricSurfaceIntersection::setTexture3DfillValue(int val)
{
    this->texture3DfillValue=val;
}

/**
 * @brief VolumetricSurfaceIntersection::getTexture3DfillValue
 * @return
 */
const int VolumetricSurfaceIntersection::getTexture3DfillValue() const
{
    return this->texture3DfillValue;
}

/**
 * @brief VolumetricSurfaceIntersection::getTextureSizeX
 */
const int VolumetricSurfaceIntersection::getTextureSizeX() const
{
    return textureSizeX;
}

/**
 * @brief VolumetricSurfaceIntersection::getTextureSizeY
 */
const int VolumetricSurfaceIntersection::getTextureSizeY() const
{
    return textureSizeY;
}

/**
 * @brief VolumetricSurfaceIntersection::getTextureSizeZ
 */
const int VolumetricSurfaceIntersection::getTextureSizeZ() const
{
    return textureSizeZ;
}


/**
 * @brief VolumetricSurfaceIntersection::getNSpheres
 */
const int VolumetricSurfaceIntersection::getNSpheres() const
{
    return nSpheres;
}

/**
 * @brief VolumetricSurfaceIntersection::getSphereRadiusMin
 */
const int VolumetricSurfaceIntersection::getSphereRadiusMin() const
{
    return sphereRadiusMin;
}

/**
 * @brief VolumetricSurfaceIntersection::getSphereRadiusMax
 */
const int VolumetricSurfaceIntersection::getSphereRadiusMax() const
{
    return sphereRadiusMax;
}

/**
 * @brief VolumetricSurfaceIntersection::setOcclusionCulling
 * @param _useOcclusionCulling
 */
void VolumetricSurfaceIntersection::setOcclusionCulling(bool _useOcclusionCulling)
{
    this->useOcclusionCulling=_useOcclusionCulling;
}

/**
 * @brief VolumetricSurfaceIntersection::setOcclusionCullingViewer
 * @param eye
 * @param FOV
 */
void VolumetricSurfaceIntersection::setOcclusionCullingViewer(const Vector3d &eye, double FOV)
{

}

/**
 * @brief VolumetricSurfaceIntersection::loadSurfaceShaders
 * @param vertexShaderName
 * @param fragmentShaderName
 * @param geometryShaderName
 */
void VolumetricSurfaceIntersection::loadSurfaceShaders(const string &vertexShaderName, const string &fragmentShaderName, const string &geometryShaderName)
{

}

/**
 * @brief VolumetricSurfaceIntersection::loadTexture3DFile
 * @param filename
 */
#include "BinVoxReader.h"
void VolumetricSurfaceIntersection::loadTexture3DFile(const string &filename)
{
    BinVoxReader r;
    r.loadBinVoxFile(filename);
    this->texture3DVolume.resize(r.voxels.size());
    this->textureSizeX = r.width;
    this->textureSizeY = r.height;
    this->textureSizeZ = r.depth;

    for (unsigned int i=0; i<r.voxels.size();i++)
        this->texture3DVolume[i] = 255*r.voxels[i];
}
