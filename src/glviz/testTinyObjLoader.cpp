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

#include "TinyObjLoader.h"
#include <iostream>
#include <cstdio>
// A good list of mesh
// http://graphics.cs.williams.edu/data/meshes.xml
using namespace std;
using namespace tinyobj;
int main(int argc, char *argv[])
{

    std::string inputfile = "../data/objmodels/face.obj";
    std::vector<tinyobj::shape_t> shapes;

    std::string err = tinyobj::LoadObj(shapes, inputfile.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return 0;
    }

    std::cout << "# of shapes : " << shapes.size() << std::endl;

    for (size_t i = 0; i < shapes.size(); i++)
    {
        cerr << "::::" << sizeof(shapes.at(i).mesh.positions) << endl;
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        //assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++)
        {
            printf("  idx[%ld] = %d\n", f, shapes[i].mesh.indices[f]);
        }

        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        //assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++)
        {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3*v+0],
                    shapes[i].mesh.positions[3*v+1],
                    shapes[i].mesh.positions[3*v+2]);
        }

        printf("shape[%ld].material.name = %s\n", i, shapes[i].material.name.c_str());
        printf("  material.Ka = (%f, %f ,%f)\n", shapes[i].material.ambient[0], shapes[i].material.ambient[1], shapes[i].material.ambient[2]);
        printf("  material.Kd = (%f, %f ,%f)\n", shapes[i].material.diffuse[0], shapes[i].material.diffuse[1], shapes[i].material.diffuse[2]);
        printf("  material.Ks = (%f, %f ,%f)\n", shapes[i].material.specular[0], shapes[i].material.specular[1], shapes[i].material.specular[2]);
        printf("  material.Tr = (%f, %f ,%f)\n", shapes[i].material.transmittance[0], shapes[i].material.transmittance[1], shapes[i].material.transmittance[2]);
        printf("  material.Ke = (%f, %f ,%f)\n", shapes[i].material.emission[0], shapes[i].material.emission[1], shapes[i].material.emission[2]);
        printf("  material.Ns = %f\n", shapes[i].material.shininess);
        //printf("  material.map_Ka = %s\n", shapes[i].material.ambientTexName.c_str());
        //printf("  material.map_Kd = %s\n", shapes[i].material.diffuseTexname.c_str());
        printf("  material.map_Ks = %s\n", shapes[i].material.specularTexname.c_str());
        printf("  material.map_Ns = %s\n", shapes[i].material.normalTexname.c_str());
        std::map<std::string, std::string>::iterator it(shapes[i].material.unknownParameter.begin());
        std::map<std::string, std::string>::iterator itEnd(shapes[i].material.unknownParameter.end());
        for (; it != itEnd; it++)
        {
            printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
        }
        printf("\n");
    }
    return 0;
}
