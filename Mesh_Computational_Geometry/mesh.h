#ifndef MESH_H
#define MESH_H

#include <QGLWidget>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <math.h>
#include <stdlib.h>

class Mesh; // Déclaration de la classe Mesh pour l'utiliser dans les iterateurs


// Classe Point (3 coordonnées dans l'espace)

class Point
{
    double _x;
    double _y;
    double _z;


public:
    Point():_x(),_y(),_z() {}
    Point(float x_, float y_, float z_):_x(x_),_y(y_),_z(z_) {}

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }

    double dot(Point p2) { return (_x*p2.x() + _y*p2.y() + _z*p2.z()); }
    Point vdot(Point p2)
    {
        Point r(_y*p2.z()-_z*p2.y(), _z*p2.x()-_x*p2.z(), _x*p2.y()-_y*p2.x());
        return r;
    }
    double norme() { return sqrt(_x*_x+_y*_y+_z*_z); }
    Point get2D() const { return Point(_x, _y, 0); }
    Point phi() const { return Point(_x, _y, _x*_x+_y*_y); }
};

Point operator-(Point p1, Point p2);
int orientedCC(Point A, Point B, Point C);

double cotan(Point p1, Point p2, Point p3);

// Classe Vertice (Sommet de face, composé d'un point et d'une face adjacente)

class Vertice
{
    Point _p;
    int _aFace;
    int _index;

public:
    Vertice():_p(), _aFace(-1), _index(-1) {}
    Vertice(float x_, float y_, float z_):_p(x_, y_, z_), _aFace(-1), _index(-1) {}

    Point p() const { return _p; }
    int aFace() const { return _aFace; }
    int index() const { return _index; }

    void setAFace(int aFace_) { _aFace = aFace_; }
    void setIndex(int index_) { _index = index_; }
};

// Classe Face (composée de 3 index de sommets, et 3 index de faces adjacentes)

class Face
{
    uint _v0,_v1,_v2;
    uint _f0,_f1,_f2;

public:
    Face():_v0(),_v1(),_v2(),_f0(),_f1(),_f2() {}
    Face(int v0_,int v1_,int v2_):_v0(v0_),_v1(v1_),_v2(v2_) {_f0 = -1; _f1 = -1; _f2 = -1;}

    uint v0() const { return _v0; }
    uint v1() const { return _v1; }
    uint v2() const { return _v2; }

    uint f0() const { return _f0; }
    uint f1() const { return _f1; }
    uint f2() const { return _f2; }

    void setv0(int v0_) { _v0 = v0_; }
    void setv1(int v1_) { _v1 = v1_; }
    void setv2(int v2_) { _v2 = v2_; }

    void setf0(int f0_) { _f0 = f0_; }
    void setf1(int f1_) { _f1 = f1_; }
    void setf2(int f2_) { _f2 = f2_; }

    double aire(Mesh mesh);
};

// Classe itérateur sur les faces

class iterator_on_faces
{
    int current_face;
    Mesh *mesh;

public:
    iterator_on_faces(int pos, Mesh &container) : current_face(pos), mesh(&container) {}

    iterator_on_faces& operator= (const iterator_on_faces& other) { current_face = other.current_face; mesh = other.mesh; return *this; }
    iterator_on_faces& operator++ () { current_face++; return *this;}
    iterator_on_faces operator++ (int) { iterator_on_faces ret = *this; current_face++; return ret;}

    bool operator!= (const iterator_on_faces& other) { return (current_face != other.current_face); }

    Face operator* ();
};

// Classe itérateur sur les vertices

class iterator_on_vertices
{
    int current_vertice;
    Mesh *mesh;

public:
    iterator_on_vertices(int pos, Mesh &container) : current_vertice(pos), mesh(&container) {}

    iterator_on_vertices& operator= (const iterator_on_vertices& other) { current_vertice = other.current_vertice; mesh = other.mesh; return *this; }
    iterator_on_vertices& operator++ () { current_vertice++; return *this;}
    iterator_on_vertices operator++ (int) { iterator_on_vertices ret = *this; current_vertice++; return ret;}

    bool operator!= (const iterator_on_vertices& other) { return (current_vertice != other.current_vertice); }

    Vertice & operator* ();
};

// Classe circulateur sur les faces (A partir d'un sommet donné, circulation sur toutes les faces adjacentes)

class circulator_on_faces
{

protected:
    int current_face;
    Vertice v;
    Mesh *mesh;

public:
    circulator_on_faces(int pos, Vertice &sommet, Mesh &container) : current_face(pos), v(sommet), mesh(&container) {}

    circulator_on_faces& operator= (const circulator_on_faces& other) { current_face = other.current_face; v = other.v; mesh = other.mesh; return *this; }
    circulator_on_faces& operator++ ();
    circulator_on_faces operator++ (int);

    bool operator!= (const circulator_on_faces& other) { return (current_face != other.current_face); }

    Face operator* ();
};

// Classe circulateur sur les faces (A partir d'un sommet donné, circulation sur toutes les faces adjacentes)

class circulator_on_vertices : public circulator_on_faces
{

public:
    circulator_on_vertices(int pos, Vertice &sommet, Mesh &container) : circulator_on_faces(pos, sommet, container) {}

    Vertice operator* ();
};

// Classe Mesh (Comprend toutes les faces et les sommets du maillage)

class Mesh
{
    QVector<Vertice> vertex;
    QVector<Face> faces;
    std::map<std::pair<int, int>, std::pair<int, int>> map;
    std::map<std::pair<int, int>, std::pair<int, int>>::iterator map_it;

    void loadFile();
public:
    Mesh();

    Face& getFace(int index) { return faces[index]; }
    Vertice& getVertice(int index) { return vertex[index]; }

    void createMesh();
    bool updateMesh();

    void drawMesh();
    void drawMeshWireFrame();

    Point laplacien(Vertice v);

    void divideTriangle(uint indexTriangle, uint newVertice);
    void flipEdge(uint indexTriangle, uint edge);
    bool isEdgeIllegal(uint indexTriangle, uint edge, int& fy);
    void legaliseEdge(uint indexTriangle, uint edge);

    int pointInTriangle(Point P, uint indiceTriangle);

    iterator_on_faces faces_begin() { return iterator_on_faces(0, *this); }
    iterator_on_faces faces_end() { return iterator_on_faces(faces.size(), *this); }

    iterator_on_vertices vertices_begin() { return iterator_on_vertices(0, *this); }
    iterator_on_vertices vertices_end() { return iterator_on_vertices(vertex.size(), *this); }

    circulator_on_faces incident_faces (Vertice & v) { return circulator_on_faces(v.aFace(), v, *this); }
    circulator_on_vertices incident_vertices (Vertice & v) { return circulator_on_vertices(v.aFace(), v, *this); }
};

#endif // MESH_H
