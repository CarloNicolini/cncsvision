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

#ifndef _GRAPHLIB_H_
#define _GRAPHLIB_H_

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdlib>

// Look here: https://github.com/wixon/De-Bruijn-Sequence
enum EdgeType { DIRECTED, UNDIRECTED };

class GraphException : public std::logic_error
{
public:
    GraphException(const std::string &message_) : std::logic_error(message_) {}
};

inline std::ostream &operator<<(std::ostream &os, const GraphException& exc) {
    return os << exc.what();
}

template<EdgeType ET>
class Graph {
public:
    typedef std::map<int, int>  Vertex;      // maps vertexnumber (from neighbour) on edgenumber
    // Constructs a Graph with given EdgeType and number of vertices (default 0), without edges.
    Graph(int n=0);

    // Returns true if EdgeType is DIRECTED, false if UNDIRECTED.
    bool isDirected() const;

    // Inserts a new 'empty' vertex, without edges.
    // Returns vertexnumber from inserted vertex (starts with 0).
    virtual int addVertex();

    // Inserts edge between vertexnumbers 'from' and 'to'.
    // Throws GraphException if edge already exists or vertexnumbers are invalid.
    // Returns edgenumber from inserted edge (start with 0).
    // With an UNDIRECTED Graph a backedge is inserted too (with same edgenumber!)
    virtual int addEdge(int from, int to);

    // Removes the edge between vertexnumbers 'from' and 'to', including the backedge if UNDIRECTED Graph.
    // Throws GraphException if vertexnumbers are invalid.
    // Other edges won't be renumbered.
    virtual void removeEdge(int from, int to);

    // Returns the number of vertices.
    int numberOfVertices() const;

    // Returns the number of edges.
    // UNDIRECTED Graph: edges won't be counted twice.
    int numberOfEdges() const;

    // Returns edgenumber from the edge between 'from' and 'to'.
    // Returns -1 if the edge doensn't exists.
    // Throws a GraphException if vertexnumbers are invalid.
    // Attention: performance is O(log(e)) where e number of edged from 'from'.
    int edgenumber(int from, int to) const;

    // Removes all vertices and edges.
    virtual void clear();

    // Access to the vertices
    inline const Vertex &operator[](int i) const {
        return vertices[i];
    }
    inline Vertex &operator[](int i)       {
        return vertices[i];    // can be used as lvalue
    }

    // Writes the complete Graph to outputstream os.
    virtual void write(std::ostream &os) const;

    // Writes the vertex with vertexnumber v to outputstream os.
    virtual void writeVertex(std::ostream &os, int v) const;

    // Writes the edge with edgenumber e to outputstream os.
    virtual void writeEdge(std::ostream &os, int e) const;


protected:
    // auxiliary functions
    void checkVertexNumber(int v) const;   // throws exception if v invalid
    void addEdgeInDatastructure(int from, int to,int edgenumber);
    void removeEdgeFromDatastructure(int from, int to);
protected:
    // data fields
    std::vector<Vertex>  vertices;
    int                 highestEdgenumber;
    EdgeType           edgetype;
    std::stack<int>     availableEdgenumbers;
};


template<EdgeType ET>
inline std::ostream &operator<<(std::ostream& os, const Graph<ET>& g);

template<EdgeType ET>
void Graph<ET>::checkVertexNumber(int k) const {
    if (k<0 || (size_t)k>=vertices.size())
        throw GraphException("invalid vertexnumber");
}

template<EdgeType ET>
Graph<ET>::Graph(int n) : vertices(n), highestEdgenumber(0)
{

}

template<EdgeType ET>
bool Graph<ET>::isDirected() const {
    return true;    //for DIRECTED Graph
}

// See why putting the declaration inline is important when using template specialization
// http://stackoverflow.com/questions/4445654/multiple-definition-of-template-specialization-when-using-different-objects
template<>
inline bool Graph<UNDIRECTED>::isDirected() const {
    return false;    //for UNDIRECTED Graph
}


template<EdgeType ET>
int Graph<ET>::addVertex() {
    int n = vertices.size();
    vertices.resize(n+1); // default constructor for new vertex
    return n;
}


template<EdgeType ET>
int Graph<ET>::addEdge(int from, int to) {
    checkVertexNumber(from);
    checkVertexNumber(to);

    if (vertices[from].count(to) > 0)
    {
        std::ostringstream out;
        out << "edge " << from << "-" << to << " already exists";
        throw GraphException(out.str());
    }
    else {
        int edgenumber;
        if (!availableEdgenumbers.empty()) {
            edgenumber=availableEdgenumbers.top();
            availableEdgenumbers.pop();
        } else
            edgenumber=highestEdgenumber++;
        addEdgeInDatastructure(from,to,edgenumber);
        return edgenumber;
    }
}

template<EdgeType ET>//for DIRECTED Graph
void Graph<ET>::addEdgeInDatastructure(int from, int to, int edgenumber) {
    vertices[from][to] = edgenumber;
}

template<>
inline void Graph<UNDIRECTED>::addEdgeInDatastructure(int from, int to, int edgenumber) {
    vertices[from][to] = edgenumber;
    vertices[to][from] = edgenumber;
}

template<EdgeType ET>
void Graph<ET>::removeEdge(int from, int to) {
    checkVertexNumber(from);
    checkVertexNumber(to);
    if (vertices[from].find(to)!=vertices[from].end()) { //verbinding bestaat
        availableEdgenumbers.push(vertices[from][to]);
        removeEdgeFromDatastructure(from,to);
    }
}

template<EdgeType ET>
void Graph<ET>::removeEdgeFromDatastructure(int from, int to) {
    vertices[from].erase(to);
}

template<>
inline void Graph<UNDIRECTED>::removeEdgeFromDatastructure(int from, int to) {
    vertices[from].erase(to);
    vertices[to].erase(from);
}

template<EdgeType ET>
int Graph<ET>::numberOfVertices() const {
    return vertices.size();
}

template<EdgeType ET>
int Graph<ET>::numberOfEdges() const {
    return highestEdgenumber-availableEdgenumbers.size();
}

template<EdgeType ET>
int Graph<ET>::edgenumber(int from, int to) const {
    checkVertexNumber(from);
    checkVertexNumber(to);
    Vertex::const_iterator it = vertices[from].find(to);

    if (it == vertices[from].end())
        return -1;
    else
        return (*it).second;
}

template<EdgeType ET>
void Graph<ET>::clear() {
    vertices.clear();
    highestEdgenumber = 0;
    while (!availableEdgenumbers.empty())
        availableEdgenumbers.pop();
}

template<EdgeType ET>
void Graph<ET>::write(std::ostream &os) const {
    os << "Graph: " << numberOfVertices() << " vertices and "
       << numberOfEdges() << " edges:" << std::endl;
    for (int v=0; v<numberOfVertices(); v++)
        writeVertex(os, v);
}

template<EdgeType ET>
void Graph<ET>::writeVertex(std::ostream &os, int v) const {
    os << "vertex " << v << ":" << std::endl;
    for (Vertex::const_iterator it=vertices[v].begin(); it!=vertices[v].end(); ++it)
    {
        os << "  ->" << it->first;
        writeEdge(os, it->second);
    }
}

template<EdgeType ET>
void Graph<ET>::writeEdge(std::ostream &os, int e) const {
    os << " via " << e << std::endl;
}

template<EdgeType ET>
inline std::ostream &operator<<(std::ostream &os, const Graph<ET> &g) {
    g.write(os);
    return os;
}

template<EdgeType ET,class Edgedata>
class GraphWithEdgedata: public virtual Graph<ET> {
public:
    GraphWithEdgedata(int n=0):Graph<ET>(n) {};
    virtual int addEdge(int from, int to);
    virtual int addEdge(int from, int to, const Edgedata&);

    const Edgedata* getEdgedata(int from,int to) const;
    Edgedata* getEdgedata(int from,int to)	  ;

    virtual void clear();


    virtual void writeEdge(std::ostream &os, int v) const;


    std::vector<Edgedata> Edgedatavector;
};

template<EdgeType ET,class Edgedata>
int GraphWithEdgedata<ET,Edgedata>::addEdge(int from, int to) {
    return this->addEdge(from,to,Edgedata());
}


template<EdgeType ET,class Edgedata>
int GraphWithEdgedata<ET,Edgedata>::addEdge(int from, int to, const Edgedata& td) {
    bool isnewedgenumber=this->availableEdgenumbers.empty();
    int edgenumber=Graph<ET>::addEdge(from,to);
    if (isnewedgenumber)
        Edgedatavector.push_back(td);
    else
        Edgedatavector[edgenumber]=td;
    return isnewedgenumber;
}

template<EdgeType ET,class Edgedata>
const Edgedata* GraphWithEdgedata<ET,Edgedata>::getEdgedata(int from,int to) const {
    int edgenumber=this->edgenumber(from,to);
    if (edgenumber!=-1)
        return &Edgedatavector[edgenumber];
    else
        return 0;
}

template<EdgeType ET,class Edgedata>
Edgedata* GraphWithEdgedata<ET,Edgedata>::getEdgedata(int from,int to) {
    int edgenumber=this->edgenumber(from,to);
    if (edgenumber!=-1)
        return &Edgedatavector[edgenumber];
    else
        return 0;
}

template<EdgeType ET,class Edgedata>
void GraphWithEdgedata<ET,Edgedata>::clear() {
    Graph<ET>::clear();
    Edgedatavector.clear();
}

template<EdgeType ET,class Edgedata>
void  GraphWithEdgedata<ET,Edgedata>::writeEdge(std::ostream &os, int v) const {
    os << " via " << v <<"(Data: "<<Edgedatavector[v]<<")"<< std::endl;
}

template<EdgeType ET, class Vertexdata>
class GraphWithVertexdata:public virtual Graph<ET> {
public:
    template<class InputIterator>
    GraphWithVertexdata(InputIterator start,InputIterator eind);
    GraphWithVertexdata():Graph<ET>() {};

    virtual int addVertex();
    virtual int addVertex(const Vertexdata&);

    const Vertexdata* getVertexdata(int knoopnr) const;
    Vertexdata* getVertexdata(int knoopnr)	  ;

    virtual void clear();
    virtual void writeVertex(std::ostream &os, int k) const;
protected:
    //datavelden
    std::vector<Vertexdata>  Vertexdatavector;
};

template<EdgeType ET, class Vertexdata>
template<class InputIterator>
GraphWithVertexdata<ET,Vertexdata>::GraphWithVertexdata(InputIterator start,InputIterator eind)
    :Graph<ET>(0) {
    for(; start!=eind; start++)
        addVertex(*start);
}

template<EdgeType ET, class Vertexdata>
int GraphWithVertexdata<ET,Vertexdata>::addVertex() {
    return this->addVertex(Vertexdata());
}

template<EdgeType ET, class Vertexdata>
int GraphWithVertexdata<ET,Vertexdata>::addVertex(const Vertexdata& kd) {
    int ret=Graph<ET>::addVertex();
    Vertexdatavector.push_back(kd);
    return ret;
}

template<EdgeType ET,class Vertexdata>
const Vertexdata* GraphWithVertexdata<ET,Vertexdata>::getVertexdata(int vertexnumber) const {
    this->checkVertexNumber(vertexnumber);
    return &Vertexdatavector[vertexnumber];
}

template<EdgeType ET,class Vertexdata>
Vertexdata* GraphWithVertexdata<ET,Vertexdata>::getVertexdata(int vertexnumber) {
    this->checkVertexNumber(vertexnumber);
    return &Vertexdatavector[vertexnumber];
}


template<EdgeType ET,class Vertexdata>
void GraphWithVertexdata<ET,Vertexdata>::clear() {
    Graph<ET>::clear();
    Vertexdatavector.clear();
}


template<EdgeType ET, class Vertexdata>
void GraphWithVertexdata<ET,Vertexdata>::writeVertex(std::ostream &os, int k) const {
    os << "Vertex " << k <<"(Data: "<<Vertexdatavector[k]<< "):" << std::endl;
    for (std::map<int, int>::const_iterator it=this->vertices[k].begin(); it!=this->vertices[k].end(); ++it) {
        os << "  ->" << it->first;
        this->writeEdge(os, it->second);
    }
}

template<EdgeType ET, class Vertexdata, class Edgedata>
class GraphWithVertexAndEdgedata:public GraphWithVertexdata<ET,Vertexdata>,
        public GraphWithEdgedata<ET, Edgedata> {
public:
    template<class InputIterator>
    GraphWithVertexAndEdgedata(InputIterator start,InputIterator eind):
        GraphWithVertexdata<ET,Vertexdata>(start,eind) {};
    GraphWithVertexAndEdgedata():
        GraphWithVertexdata<ET,Vertexdata>() {};
    virtual void clear() {
        GraphWithVertexdata<ET,Vertexdata>::clear();
        this->Edgedatavector.clear();
    }
};


template<EdgeType ET>
class Eulercircuit: public virtual Graph<ET>
{
public:
    typedef std::map<int, int>  Vertex;
    std::stack<int> q;
    std::vector<unsigned int> circuit;
    bool randomizeEulerianCircuits;
    Eulercircuit<ET>(int n=0) : Graph<ET>(n)
    {
        randomizeEulerianCircuits=false;
    }

    void generate(int kn)
    {
        while(tour(kn)==kn && !q.empty())
        {
            kn=q.top();
            circuit.push_back(kn);
            q.pop();
        }
    }

    int tour(int v)
    {
        int l=v;
        while(true)
        {
            Vertex k = this->vertices[l];
            std::map<int,int>::iterator it = k.begin();
            int nneighbors = k.size();
            if (nneighbors>0)
            {
                if (this->randomizeEulerianCircuits)
                    std::advance(it, int(rand()%(nneighbors)));
            }
            if (it==k.end())
                break;
            int w = it->first;
            q.push(l);
            this->removeEdge(l,w);
            l=w;
        }
        return l;
    }
};
/*
class DeBruijnsequence: public std::vector<char>
{
public:
    DeBruijnsequence(int _m, int _n):m(_m), n(_n-1)
    {
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
                }
            }
        }
        
        for(unsigned int i = 0; i < from.size(); i++)
        {
            graph.addEdge(from.at(i), to.at(i), c.at(i));
        }

        //std::vector<int> temp;
        //Eulercircuit<DIRECTED> euler(graph,temp);
        //for(int i = temp.size()-1; i > 0; i--)
        //{
        //    push_back(*graph.getEdgedata(temp.at(i), temp.at(i - 1)));
        //}
        //push_back(*graph.getEdgedata(temp.at(0), temp.at(temp.size()-1)));

    };
    friend std::ostream &operator<<(std::ostream &os, const DeBruijnsequence &b);
private:
    const int n;
    const int m;
};
*/
#endif
