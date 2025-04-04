#ifndef OCTREE_H
#define OCTREE_H
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <GL/glut.h>
using namespace std;

struct Point{
    int x;
    int y;
    int z;
    bool init;
    
    Point() : x(0), y(0), z(0), init(false) {}
    Point(int a, int b, int c) : x(a), y(b), z(c), init(true) {}
    Point& operator=(const Point &Ptr) {
        x = Ptr.x;
        y = Ptr.y;
        z = Ptr.z;
        init = Ptr.init;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point &p) {
        os << "(" << p.x<<", "<<p.y<<", "<<p.z<< ")";
        return os;
    }

    bool operator==(const Point& p) const {
        return (x == p.x && y == p.y && z == p.z);
    }

    bool inside(const Point &p, int h) const;
    double distance(const Point &p) const;
};


class Octree {
    private:
        Octree *children[8];
        //Point *points; // 
        std::vector<Point> points;

        // bottomLeft y h definen el espacio(cubo más grande)
        Point bottomLeft;
        double h;
        int nPoints; // puntos ingresados o capacidad.
        void find_closest(const Point &target, int radius, Point &closestPoint, double &minDist);

    public:
        Octree();
        Octree(const Point &p, double height, int capacity);
        bool exist( const Point & );
        void insert( const Point & );
        Point find_closest(const Point &, int radius);
        void printTree(std::string line="|");
        void get_h_bottom(const Point &p);
        
        void drawCube(const Point , double h);
        void drawOctree();
};

#endif
