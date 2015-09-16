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

#include "Graph.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    int m=2;
    int n=1;

    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

    int count = (int) std::pow((double)m, (double)n);
    std::vector<std::string> vertices(count);
    for(int i = 0; i < count; i++)
    {
        int x = i;
        for(int j = 0; j < n; j++)
        {
            vertices.at(i) += alphabet.at(x % m);
            x /= m;
        }
    }

    GraphWithVertexAndEdgedata<DIRECTED, std::string, char> graph;
    std::vector<int> from;
    std::vector<int> to;
    std::vector<char> c;
    for( int i = 0; i < count; i++)
    {
        graph.addVertex(vertices.at(i));
        for( int j = 0; j < count; j++)
        {
            if( vertices.at(i).substr(1, n-1) == vertices.at(j).substr(0, n-1))
            {
                from.push_back(i);
                to.push_back(j);
                c.push_back(vertices.at(j).at(n-1));
                cerr << "Adding edge " << i << "-" << j << " " << vertices.at(j).at(n-1) << endl;
            }
        }
    }

    for(int i = 0; i < from.size(); i++)
    {
        graph.addEdge(from.at(i), to.at(i), c.at(i));
    }

    graph.write(cerr);

    srand(time(0));

    for (int k=0; k<10; k++)
    {
        Eulercircuit<DIRECTED> eulerian(graph.numberOfVertices());
        for (int i=0; i< from.size(); i++)
            eulerian.addEdge(from.at(i),to.at(i));
        eulerian.generate(k%eulerian.numberOfVertices());
        for (int l=0; l<eulerian.circuit.size();l++)
            cout << eulerian.circuit.at(l) << " " ;
        cout << endl ;
    }

}
