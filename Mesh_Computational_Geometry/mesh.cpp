#include "mesh.h"

Mesh::Mesh()
{
    loadFile();
}

void Mesh::loadFile()
{
    std::ifstream fichier("/Users/Raton/Documents/SecteurInfo/CoursGraphiques/TP1/terrains/franke4.off");
    //std::ifstream fichier("/Users/Raton/Documents/SecteurInfo/CoursGraphiques/TP1/queen2.off");
    //std::ifstream fichier("/Users/Raton/Documents/SecteurInfo/CoursGraphiques/TP1/terrains/test.off");

    if (fichier.is_open()) // ce test échoue si le fichier n'est pas ouvert
    {
        std::string ligne; // variable contenant chaque ligne lue

        std::getline(fichier, ligne, ' ');
        int nVertex = std::stoi(ligne);

        std::getline(fichier, ligne, ' ');
        int nFaces = std::stoi(ligne);

        std::getline(fichier, ligne);

        for (int i=0; i<nVertex; i++) // On récupère les sommets
        {
            std::getline(fichier, ligne, ' ');
            float s1 = std::stof(ligne);
            std::getline(fichier, ligne, ' ');
            float s2 = std::stof(ligne);
            std::getline(fichier, ligne);
            float s3 = std::stof(ligne);

            Vertice v(s1, s2, s3);
            v.setIndex(i);
            vertex.push_back(v);
        }

        // Si pas de faces, on en ajoute 2 qui englobent tous les points

        if (nFaces <= 0)
        {
            float xmax, xmin, ymax, ymin;

            for (int j = 0; j<vertex.size(); j++)
            {
                if (vertex[j].p().x() < xmin)
                    xmin = vertex[j].p().x();
                if (vertex[j].p().x() > xmax)
                    xmax = vertex[j].p().x();
                if (vertex[j].p().y() < ymin)
                    ymin = vertex[j].p().y();
                if (vertex[j].p().y() > ymax)
                    ymax = vertex[j].p().y();
            }

            Vertice v0(xmin, ymin, 0);
            v0.setAFace(0);
            v0.setIndex(vertex.size());
            Vertice v1(xmin, ymax, 0);
            v1.setAFace(0);
            v1.setIndex(vertex.size()+1);
            Vertice v2(xmax, ymin, 0);
            v2.setAFace(0);
            v2.setIndex(vertex.size()+2);
            Vertice v3(xmax, ymax, 0);
            v3.setAFace(1);
            v3.setIndex(vertex.size()+3);

            vertex.push_back(v0);
            vertex.push_back(v1);
            vertex.push_back(v2);
            vertex.push_back(v3);

            Face f1(vertex.size()-4, vertex.size()-2, vertex.size()-3);
            Face f2(vertex.size()-2, vertex.size()-1, vertex.size()-3);
            f1.setf0(1);
            f2.setf1(0);

            faces.push_back(f1);
            faces.push_back(f2);
        }


        for (int j=0; j<nFaces; j++) // On récupère les faces (3 index de sommets)
        {
            std::getline(fichier, ligne, ' '); // 3
            std::getline(fichier, ligne, ' '); // V0
            int v1Index = std::stoi(ligne);
            std::getline(fichier, ligne, ' '); // V1
            int v2Index = std::stoi(ligne);
            std::getline(fichier, ligne); // V2
            int v3Index = std::stoi(ligne);

            if (vertex[v1Index].aFace() == -1)
                vertex[v1Index].setAFace(j);
            if (vertex[v2Index].aFace() == -1)
                vertex[v2Index].setAFace(j);
            if (vertex[v3Index].aFace() == -1)
                vertex[v3Index].setAFace(j);

            faces.push_back(Face(v1Index, v2Index, v3Index));

            map_it = map.find(std::make_pair(v2Index, v1Index));
            if (map_it == map.end())
                map.insert(std::make_pair(std::make_pair(v1Index, v2Index), std::make_pair(j, 2)));
            else
            {
                faces[j].setf2(map[std::make_pair(v2Index, v1Index)].first);
                if (map[std::make_pair(v2Index, v1Index)].second == 0)
                    faces[map[std::make_pair(v2Index, v1Index)].first].setf0(j);
                else if (map[std::make_pair(v2Index, v1Index)].second == 1)
                    faces[map[std::make_pair(v2Index, v1Index)].first].setf1(j);
                else
                    faces[map[std::make_pair(v2Index, v1Index)].first].setf2(j);
                map.erase(map_it);
            }

            map_it = map.find(std::make_pair(v3Index, v2Index));
            if (map_it == map.end())
                map.insert(std::make_pair(std::make_pair(v2Index, v3Index), std::make_pair(j, 0)));
            else
            {
                faces[j].setf0(map[std::make_pair(v3Index, v2Index)].first);
                if (map[std::make_pair(v3Index, v2Index)].second == 0)
                    faces[map[std::make_pair(v3Index, v2Index)].first].setf0(j);
                else if (map[std::make_pair(v3Index, v2Index)].second == 1)
                    faces[map[std::make_pair(v3Index, v2Index)].first].setf1(j);
                else
                    faces[map[std::make_pair(v3Index, v2Index)].first].setf2(j);
                map.erase(map_it);
            }

            map_it = map.find(std::make_pair(v1Index, v3Index));
            if (map_it == map.end())
                map.insert(std::make_pair(std::make_pair(v3Index, v1Index), std::make_pair(j, 1)));
            else
            {
                faces[j].setf1(map[std::make_pair(v1Index, v3Index)].first);
                if (map[std::make_pair(v1Index, v3Index)].second == 0)
                    faces[map[std::make_pair(v1Index, v3Index)].first].setf0(j);
                else if (map[std::make_pair(v1Index, v3Index)].second == 1)
                    faces[map[std::make_pair(v1Index, v3Index)].first].setf1(j);
                else
                    faces[map[std::make_pair(v1Index, v3Index)].first].setf2(j);
                map.erase(map_it);
            }
        }
    }
    else
        std::cout << "File not open" << std::endl;
}

void Mesh::createMesh()
{
    while (!updateMesh()) {}
    return;
}

bool Mesh::updateMesh()
{
    for (int i=0; i<vertex.size(); i++)
    {
        if (vertex[i].aFace() == -1) // Si le sommet n'est pas connecté au maillage
        {
            for (int j=0; j<faces.size(); j++)
            {
                if (pointInTriangle(vertex[i].p(), j) >= 0)
                {
                    divideTriangle(j, i);
                    vertex[i].setAFace(j);
                    legaliseEdge(j, i);
                    legaliseEdge(faces.size()-2, i);
                    legaliseEdge(faces.size()-1, i);
                    return false;
                }
            }
        }
    }
    std::cout << "No more vertex to add " << std::endl;
    return true;
}

void glVertexDraw(const Point & p) {
    glVertex3f(p.x(), p.y(), p.z());
}

void Mesh::drawMesh() {
    for(int i = 0; i < faces.size(); i++) {
        glBegin(GL_TRIANGLES);
        glColor3d(i/(1.*faces.size()),i/(1.*faces.size()),i/(1.*faces.size()));
        glVertexDraw(vertex[faces[i].v0()].p());
        glColor3d(i/(1.*faces.size()),i/(1.*faces.size()),i/(1.*faces.size()));
        glVertexDraw(vertex[faces[i].v1()].p());
        glColor3d(i/(1.*faces.size()),i/(1.*faces.size()),i/(1.*faces.size()));
        glVertexDraw(vertex[faces[i].v2()].p());
        glEnd();

        /*glBegin(GL_LINE_LOOP);
            glVertexDraw(vertex[faces[i].v0()].p());
            glVertexDraw(vertex[faces[i].v1()].p());
            glVertexDraw(vertex[faces[i].v2()].p());
        glEnd();*/
    }
}

void Mesh::drawMeshWireFrame() {
    for (int i = 0; i<vertex.size(); i++)
    {
        glBegin(GL_POINTS);
            glVertexDraw(vertex[i].p());
        glEnd();
    }
}

Face iterator_on_faces::operator *()
{
    return mesh->getFace(current_face);
}

Vertice& iterator_on_vertices::operator *()
{
    return mesh->getVertice(current_vertice);
}

Face circulator_on_faces::operator *()
{
    return mesh->getFace(current_face);
}

circulator_on_faces& circulator_on_faces::operator++ () {
    if (mesh->getFace(current_face).v0() == v.index())
        current_face = mesh->getFace(current_face).f1();
    else if (mesh->getFace(current_face).v1() == v.index())
        current_face = mesh->getFace(current_face).f2();
    else if (mesh->getFace(current_face).v2() == v.index())
        current_face = mesh->getFace(current_face).f0();
    else
        std::cout << "rate" << std::endl;
    return *this;
}

circulator_on_faces circulator_on_faces::operator++ (int) {
    circulator_on_faces ret = *this;
    if (mesh->getFace(current_face).v0() == v.index())
        current_face = mesh->getFace(current_face).f1();
    else if (mesh->getFace(current_face).v1() == v.index())
        current_face = mesh->getFace(current_face).f2();
    else if (mesh->getFace(current_face).v2() == v.index())
        current_face = mesh->getFace(current_face).f0();
    else
        std::cout << "rate" << std::endl;
    return ret;
}

Vertice circulator_on_vertices::operator *()
{
    if (mesh->getFace(current_face).v0() == v.index())
        return mesh->getVertice(mesh->getFace(current_face).v1());
    else if (mesh->getFace(current_face).v1() == v.index())
        return mesh->getVertice(mesh->getFace(current_face).v2());
    else
        return mesh->getVertice(mesh->getFace(current_face).v0());
}

Point operator-(Point p1, Point p2)
{
    Point r(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z());
    return r;
}

double cotan(Point p1, Point p2, Point p3)
{
    Point u = p1 - p2;
    Point v = p3 - p2;

    double x = u.dot(v)/(u.norme()*v.norme());
    return x/sqrt(1-x*x);
}


double Face::aire(Mesh mesh)
{
    Point u = mesh.getVertice(_v0).p() - mesh.getVertice(_v1).p();
    Point v = mesh.getVertice(_v2).p() - mesh.getVertice(_v1).p();
    return u.vdot(v).norme()/2;
}

Point Mesh::laplacien(Vertice v)
{
    double rx = 0;
    double ry = 0;
    double rz = 0;

    circulator_on_faces cfbegin = incident_faces(v);
    circulator_on_faces cf = cfbegin;

    Point p1, p2, p3;

    for (cf=cfbegin,++cf; cf!=cfbegin; cf++)
    {
        if ((*cf).v0() == v.index())
        {
            p1 = getVertice((*cf).v0()).p();
            p2 = getVertice((*cf).v1()).p();
            p3 = getVertice((*cf).v2()).p();
        }
        else if ((*cf).v1() == v.index())
        {
            p1 = getVertice((*cf).v1()).p();
            p2 = getVertice((*cf).v2()).p();
            p3 = getVertice((*cf).v0()).p();
        }
        else
        {
            p1 = getVertice((*cf).v2()).p();
            p2 = getVertice((*cf).v0()).p();
            p3 = getVertice((*cf).v1()).p();
        }

        rx += (cotan(p1, p2, p3)*(p3.x()-p1.x()) + cotan(p1, p3, p2)*(p2.x()-p1.x()))*3/(2*(*cf).aire(*this));
        ry += (cotan(p1, p2, p3)*(p3.y()-p1.y()) + cotan(p1, p3, p2)*(p2.y()-p1.y()))*3/(2*(*cf).aire(*this));
        rz += (cotan(p1, p2, p3)*(p3.z()-p1.z()) + cotan(p1, p3, p2)*(p2.z()-p1.z()))*3/(2*(*cf).aire(*this));
    }

    return Point(rx, ry, rz);
}

void Mesh::divideTriangle(uint indexTriangle, uint newVertice)
{
    int f0 = faces[indexTriangle].f0(), f1 = faces[indexTriangle].f1();
    int v0 = faces[indexTriangle].v0(), v1 = faces[indexTriangle].v1(), v2 = faces[indexTriangle].v2();

    Face f3(newVertice, v1, v2);
    Face f4(newVertice, v2, v0);

    f3.setf0(f0);
    f3.setf1(faces.size()+1);
    f3.setf2(indexTriangle);

    f4.setf0(f1);
    f4.setf1(indexTriangle);
    f4.setf2(faces.size());

    faces[indexTriangle].setv2(newVertice);
    faces[indexTriangle].setf0(faces.size());
    faces[indexTriangle].setf1(faces.size()+1);

    faces.push_back(f3);
    faces.push_back(f4);

    vertex[v0].setAFace(indexTriangle);
    vertex[v1].setAFace(indexTriangle);
    vertex[v2].setAFace(faces.size()-1);
    vertex[newVertice].setAFace(indexTriangle);

    if (f1 != -1 && faces[f1].f0() == indexTriangle)
        faces[f1].setf0(faces.size()-1);
    else if (f1 != -1 && faces[f1].f1() == indexTriangle)
        faces[f1].setf1(faces.size()-1);
    else if (f1 != -1)
        faces[f1].setf2(faces.size()-1);

    if (f0 != -1 && faces[f0].f0() == indexTriangle)
        faces[f0].setf0(faces.size()-2);
    else if (f0 != -1 && faces[f0].f1() == indexTriangle)
        faces[f0].setf1(faces.size()-2);
    else if (f0 != -1)
        faces[f0].setf2(faces.size()-2);
}

void Mesh::flipEdge(uint indexTriangle, uint edge)
{
    int fy, f2, f3;
    int vx = edge, vy, v1, v2;

    // Prepare fx

    if (edge == faces[indexTriangle].v0())
    {
        fy = faces[indexTriangle].f0();
        f3 = faces[indexTriangle].f2();

        v1 = faces[indexTriangle].v1();
        v2 = faces[indexTriangle].v2();
    }
    else if (edge == faces[indexTriangle].v1())
    {
        fy = faces[indexTriangle].f1();
        f3 = faces[indexTriangle].f0();

        v1 = faces[indexTriangle].v2();
        v2 = faces[indexTriangle].v0();
    }
    else
    {
        fy = faces[indexTriangle].f2();
        f3 = faces[indexTriangle].f1();

        v1 = faces[indexTriangle].v0();
        v2 = faces[indexTriangle].v1();
    }

    if (fy == -1) return; // Pas de face en face de l'arrête à flip (donc pas de flip)

    // Prepare fy

    if (indexTriangle == faces[fy].f0())
    {
        f2 = faces[fy].f2();
        vy = faces[fy].v0();
    }
    else if (indexTriangle == faces[fy].f1())
    {
        f2 = faces[fy].f0();
        vy = faces[fy].v1();
    }
    else
    {
        f2 = faces[fy].f1();
        vy = faces[fy].v2();
    }

    // Reset fx

    if (edge == faces[indexTriangle].v0())
    {
        faces[indexTriangle].setv1(vy);
        faces[indexTriangle].setf0(f2);
        faces[indexTriangle].setf2(fy);
    }
    else if (edge == faces[indexTriangle].v1())
    {
        faces[indexTriangle].setv2(vy);
        faces[indexTriangle].setf1(f2);
        faces[indexTriangle].setf0(fy);
    }
    else
    {
        faces[indexTriangle].setv0(vy);
        faces[indexTriangle].setf2(f2);
        faces[indexTriangle].setf1(fy);
    }

    // Reset fy

    if (indexTriangle == faces[fy].f0())
    {
        faces[fy].setv1(vx);
        faces[fy].setf0(f3);
        faces[fy].setf2(indexTriangle);
    }
    else if (indexTriangle == faces[fy].f1())
    {
        faces[fy].setv2(vx);
        faces[fy].setf1(f3);
        faces[fy].setf0(indexTriangle);
    }
    else
    {
        faces[fy].setv0(vx);
        faces[fy].setf2(f3);
        faces[fy].setf1(indexTriangle);
    }

    // Reset for old edge vertices just in case

    vertex[v1].setAFace(indexTriangle);
    vertex[v2].setAFace(fy);
}

bool Mesh::isEdgeIllegal(uint indexTriangle, uint edge, int& fy)
{
    Point A, B, C, S;

    if (faces[indexTriangle].v0() == edge)
    {
        A = vertex[faces[indexTriangle].v0()].p();
        B = vertex[faces[indexTriangle].v1()].p();
        C = vertex[faces[indexTriangle].v2()].p();

        fy = faces[indexTriangle].f0();
    }
    else if (faces[indexTriangle].v1() == edge)
    {
        A = vertex[faces[indexTriangle].v1()].p();
        B = vertex[faces[indexTriangle].v2()].p();
        C = vertex[faces[indexTriangle].v0()].p();

        fy = faces[indexTriangle].f1();
    }
    else
    {
        A = vertex[faces[indexTriangle].v2()].p();
        B = vertex[faces[indexTriangle].v0()].p();
        C = vertex[faces[indexTriangle].v1()].p();

        fy = faces[indexTriangle].f2();
    }

    if (fy == -1) return false; // Si pas de face adjacente, alors l'arrête est forcément légale

    if (faces[fy].f0() == indexTriangle)
        S = vertex[faces[fy].v0()].p();
    else if (faces[fy].f1() == indexTriangle)
        S = vertex[faces[fy].v1()].p();
    else
        S = vertex[faces[fy].v2()].p();

    double r = ((A.phi()-C.phi()).vdot((B.phi()-C.phi()))).dot((S.phi()-C.phi()));

    return r < 0;
}

void Mesh::legaliseEdge(uint indexTriangle, uint edge)
{
    int fy;

    if(isEdgeIllegal(indexTriangle, edge, fy))
    {
        flipEdge(indexTriangle, edge);
        legaliseEdge(indexTriangle, edge);
        legaliseEdge(fy, edge);
    }
}

int orientedCC(Point A, Point B, Point C)
{
    double r = (B.x()-A.x())*(C.y()-A.y())-(B.y()-A.y())*(C.x()-A.x());
    return (r>0) - (r<0);
}

int Mesh::pointInTriangle(Point P, uint indiceTriangle)
{
    Point A = vertex[faces[indiceTriangle].v0()].p();
    Point B = vertex[faces[indiceTriangle].v1()].p();
    Point C = vertex[faces[indiceTriangle].v2()].p();

    int d = orientedCC(A, B, P) + orientedCC(B, C, P) + orientedCC(C, A, P);

    if (d == 3 || d == -3)
        return 1;

    return -1;
}
