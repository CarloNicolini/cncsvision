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

#include <stdexcept>
#include "BinVoxReader.h"
using namespace std;

BinVoxReader::BinVoxReader()
{

}

BinVoxReader::BinVoxReader(const string &filespec)
{
    this->loadBinVoxFile(filespec);
}

int BinVoxReader::loadBinVoxFile(const std::string &filespec)
{

    ifstream *input = new ifstream(filespec.c_str(), ios::in | ios::binary);

    // read header
    string line;
    *input >> line;  // #binvox
    if (line.compare("#binvox") != 0)
    {
        delete input;
        std::string error1 = "Error: first line reads [" + line +"] instead of [#binvox]";
        throw std::runtime_error(error1);
        return 0;
    }
    *input >> version;
    cerr << "[BinVoxReader LOG] Reading binvox version " << version << endl;

    depth = -1;
    int done = 0;
    while(input->good() && !done)
    {
        *input >> line;
        if (line.compare("data") == 0) done = 1;
        else if (line.compare("dim") == 0)
        {
            *input >> depth >> height >> width;
        }
        else if (line.compare("translate") == 0)
        {
            *input >> tx >> ty >> tz;
        }
        else if (line.compare("scale") == 0)
        {
            *input >> scale;
        }
        else
        {
            cerr << "[BinVoxReader LOG] Unrecognized keyword [" << line << "], skipping" << endl;
            char c;
            do
            {  // skip until end of line
                c = input->get();
            }
            while(input->good() && (c != '\n'));

        }
    }
    if (!done)
    {
        throw std::runtime_error("[BinVoxReader ERROR] error reading header");
        return 0;
    }
    if (depth == -1)
    {
        throw std::runtime_error("[BinVoxReader ERROR] missing dimensions in header");
        return 0;
    }

    size = width * height * depth;
    voxels.resize(size);

    // read voxel data
    unsigned char value;
    unsigned char count;
    int index = 0;
    int end_index = 0;
    int nr_voxels = 0;

    input->unsetf(ios::skipws);  // need to read every unsigned char now (!)
    *input >> value;  // read the linefeed char

    while((end_index < size) && input->good())
    {
        *input >> value >> count;
        if (input->good())
        {
            end_index = index + count;
            if (end_index > size) return 0;
            for(int i=index; i < end_index; i++) voxels[i] = value;

            if (value) nr_voxels += count;
            index = end_index;
        }  // if file still ok

    }  // while

    input->close();
    cerr << "[BinVoxReader LOG]  read " << nr_voxels << " voxels" << endl;
    cerr << "[BinVoxReader LOG]  Tx,Ty,Tz " << tx << " " << ty << " " << tz << endl;
    cerr << "[BinVoxReader LOG]  nX,nY,nZ " << width << " " << height << " " << depth << endl;
    return 1;
}
