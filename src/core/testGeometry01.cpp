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
#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Mathcommon.h"
#include "RigidBody.h"
#include "EulerExtractor.h"
#include "Timer.h"
#include "CoordinatesExtractor.h"
#include "CoordinatesExtractor2.h"
#include "Screen.h"
#include "MatrixStream.h"
#include "Marker.h"
#include "DaisyFilter.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

#define NPOINTS 1E2

void randomizeSeed(void)
{  int stime;
   long ltime;
   ltime = time(NULL);
   stime = (unsigned) ltime / 2;
   srand(stime);
}


int main(int argc, char *argv[])
{

   // Test all the point3D functionalities
   randomizeSeed();

   CoordinatesExtractor coords;
   CoordinatesExtractor2 coords2;
   coords2.setFilterVelocity(true,0.9);

   double iod=1;
   Vector3d p1 = Vector3d(0,1.0,0);
   Vector3d p2 = Vector3d(0,-1.0,0);

   vector<Marker> v;
   v.push_back(Marker(Vector3d(0,1,2)));
   v.push_back(Marker(Vector3d(10,1,2)));
   v.push_back(Marker(Vector3d(0,14,2)));
   v.push_back(Marker(Vector3d(0,1,52)));

   coords2.init(p1,p2,Vector3d::UnitX(),Vector3d::UnitY(),Vector3d::UnitZ(),100);

   double deltaT=1.0/120.0;
   for (double i=0; i<M_PI/10; i+=deltaT)
   {  AngleAxis<double> aa(i*2*M_PI,Vector3d(0,0,1) );
      Matrix3d R = aa.toRotationMatrix();
      Affine3d A = Affine3d::Identity();
      A.linear()= R;
      A.translation() = Vector3d(i*i,0,0);
      Vector3d r1 = A*Vector3d::UnitX();
      Vector3d r2 = A*Vector3d::UnitY();
      Vector3d r3 = A*Vector3d::UnitZ();

      coords2.update( r1,r2,r3,deltaT);
      Marker X1 = coords2.getRightEye();
      Marker X2 = coords2.getLeftEye();
      //Marker X2 = coords2.getP2();
      //Marker X2 = coords2.getRightEye();
      //cout << X1.p.transpose() << "\t" << X2.p.transpose() << endl;
      //cout << X1.p.transpose() << endl;
      if ( i> 0.1 )
      {  //cout << X1.p.transpose() << "\t" << X1.v.transpose() << "\t" << X1.a.transpose() << "\t" << endl;
         cerr << (X1.p - X2.p).norm() << endl;
         //cout << X2.p.transpose() << "\t" <<  X2.v.transpose() << "\t" << X2.a.transpose() << endl;
      }
      //cerr << toDegrees(acos( X1.v.normalized().dot(X1.a.normalized()))) << "deg " << toDegrees(acos( X1.v.normalized().dot(X1.p.normalized()))) << " deg" << endl;
   }

   return 0;
}

/*
   Point3D c0(1,0,0);
   Point3D c1(0,1,0);
   Point3D c2(0,0,1);

   Matrix3d I3 = Matrix3d::Identity();
   Matrix4d I4 = Matrix4d::Identity();

   Vector3d v3(1,2,3);
   double factor=2;

   cout << "====== Here are all the methods of the class Point3D ===== " << endl;
   cout << "Product by scalar " << c0*factor << endl;
   cout << "Product by 3x3 matrix" << c0*I3 << endl;
   cout << "Product by 4x4 matrix " << c0*factor << endl;
   cout << "Division by scale " << c0/factor << endl;
   cout << "Sum p1+p2 " << c0+c1 << endl;
   cout << "Sum p1+p2 " << c0+c1 << endl;
   cout << "Sum p1+scalar " << c0+factor << endl;
   cout << "Difference p1-p2 " << c0-c1 << endl;
   cout << "Difference p1-scalar " << c0-factor << endl;
   cout << "Random access " << c0[0] << ", " << c0[1] << ", " << c0[2] << endl;
   cout << "Cross product " << (c0^c1) << endl;

   cout << "===== Modifiers =====" << endl;
   c1=c0;
   c0*=factor;
   cout << "*= double " << c0 << endl;
   c0=c1;
   c0*=I3;
   cout << "*= Matrix3d " << c0 << endl;
   c0=c1;
   c0*=I4;
   cout << "*= Matrix4d " << c0 << endl;
   c0=c1;
   c0/=factor;
   cout << "/= double " << c0 << endl;
   c0=c1;
   c0+=c1;
   cout << "+= Point3D " << c0 << endl;
   c0=c1;
   c0-=c1;
   cout << "-= Point3D "  << c0 << endl;
   c0=c1;
   c0=Point3D::xAxis();
   c1=Point3D::yAxis();
   c0^=c1;
   cout << "^= (cross product) " << c0 << endl;

   cout << "== Point3D " << (c0==c1) << endl;
   cout << "!= Point3D " << (c0!=c1) << endl;
   cout << "Lenght= " << c0.length() << endl;
   cout << "Length^2= " << c0.length2() << endl;
   cout << "Distance= " << c0.distance(c1) << endl;
   cout << "Normalized= " << c0.normalized() << endl;
   cout << "LinearInterp (start) " << c0.lerp(c1,0) << endl;
   cout << "LinearInterp (end) " << c0.lerp(c1,1) << endl;
   cout << "SphericalInterp (start) " << c0.slerp(c1,0) << endl;
   cout << "Testing spherical interpolation" << endl;
   Point3D start(1,1,0);
   Point3D end(0,1,0);
   for (double t=0; t<=1.05; t+=0.05)
   {
      cout << start.slerp(end,t) << endl;
   }

   cout << "SphericalInterp (end) " << c0.slerp(c1,1) << endl;
   cout << "Inverse= " << c0.inverse() << endl;
   cout << "Cross=  " << c0.cross(c2) << endl;
   cout << "NormalizeInPlace= " << c0.normalized() << endl;

   c0=Point3D::xAxis();
   c0.rotateX(M_PI/4);
   cout << "Rotated 45° around (1,0,0)= " << c0 << endl;
   c0=Point3D::xAxis();
   c0.rotateY(M_PI/4);
   cout << "Rotated 45° around (0,1,0)= " << c0 << endl;
   c0=Point3D::xAxis();
   c0.rotateZ(M_PI/4);
   cout << "Rotated 45° around (0,1,0)= " << c0 << endl;
   c0=Point3D::xAxis();
   c0.rotate(Point3D(1,1,1),M_PI/4.);
   cout << "Rotated 45° around (1,1,1)= " << c0 << endl;
   c0=Point3D::xAxis();
   c1=Point3D::yAxis();
   cout << "Dot x.y= " << c0.dot(c1) << endl;

   cout << "=== Now the hardest part, system of coordinates changing ====" << endl;

   Point3D x0 = Point3D::xAxis();
   Point3D y0 = Point3D::yAxis();
   Point3D z0 = Point3D::zAxis();

   Point3D x1 = Point3D::xAxis();
   Point3D y1 = Point3D::yAxis();
   Point3D z1 = Point3D::zAxis();
   x1.rotateZ(-M_PI/2.);
   y1.rotateZ(-M_PI/2.);
   //z1.rotateZ(M_PI/4.);
   I3 = Matrix3d::Identity();
   I3(0,0)=x0.dot(x1);
   I3(0,1)=x0.dot(y1);
   I3(0,2)=x0.dot(z1);

   I3(1,0)=y0.dot(x1);
   I3(1,1)=y0.dot(y1);
   I3(1,2)=y0.dot(z1);

   I3(2,0)=z0.dot(x1);
   I3(2,1)=z0.dot(y1);
   I3(2,2)=z0.dot(z1);

   Point3D traslation(1,2,3);
   Point3D p1(2,1,0);

   cout << I3 << endl;
   ReferenceFrame Sp;
   ReferenceFrame Sq;

   Sq.setReferenceFrame(I3,traslation);

   Sp.computeRelativeTransformations(Sq);

   Point3D p2 = p1.changeReferenceFrame(Sp.rotationWC, Sp.traslation);

   cout << "r vector as viewed from S1 " << p1 << endl;
   cout << "r vector as viewed from S2 " << p2 << endl;

   cout << "TESTING ROTATIONS" << endl;

   ReferenceFrame *O1,*O2;
   O1=new ReferenceFrame();
   O2=new ReferenceFrame();

   Matrix3d axes;
   axes << sqrt(2)/2, sqrt(2)/2, 0, sqrt(2)/2, -sqrt(2)/2, 0, 0, 0, 1;
   Vector3d t(1,1,0);
   O2->setAxes(axes);
   O2->settraslation(t);
   O2->computeRelativeTransformations(*O1);

   cerr << *O2 << endl;
   Point3D x(1,0,0);
   cerr << x << endl << x.changeReferenceFrame(O2->rotationCW,O2->traslation) << endl;

   delete O1;
   delete O2;
*/

