#include "Octree.hpp"

bool Point::inside(const Point &p, int h) const{
    return p.x>=x && p.x<x+h &&
            p.y>=y && p.y<y+h &&
            p.z>=z && p.z<z+h;
}

double  Point::distance(const Point &p) const{
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
}

Octree::Octree(const Point &p, double height, int capacity){
    bottomLeft = p;
    h = height;
    nPoints = capacity;

    for (int i = 0; i < 8; i++){
        children[i] = nullptr;
    }
}

bool Octree::exist(const Point& p) {
    for (const auto& point : points) { 
        if (point == p) 
            return true;
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr && children[i]->exist(p))
            return true;
    }

    return false;
}


void Octree::insert(const Point &new_point){
    if(!bottomLeft.inside(new_point, h) || exist(new_point)){
        return;
    }

    if(points.size()<nPoints){
        points.push_back(new_point);
    }
    else{
        if (children[0] == nullptr) {
            double mid_h = h / 2;
            int index = 0;

            for (int i = 0; i <2; i++) {
                for (int j = 0; j <2; j++) {
                    for (int k = 0; k <2; k++) {
                        Point childBottomLeft(
                            bottomLeft.x + k * mid_h,
                            bottomLeft.y + j * mid_h,
                            bottomLeft.z + i * mid_h
                        );
                        children[index++] = new Octree(childBottomLeft, mid_h, nPoints);
                    }
                }
            }
        }

        for (int i = 0; i < 8; i++) {
            if (children[i]->bottomLeft.inside(new_point, children[i]->h)) {
                children[i]->insert(new_point);
                return;
            }
        }

    }
}

void Octree::printTree(std::string line) {
    if (points.size()) {
      std::cout << line;
      for(int i=0; i<points.size(); i++){
        std::cout<<points[i]<<" "; //" bottom: "<<bottomLeft<<" ";
      }
      std::cout<<std::endl;
    }
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            children[i]->printTree(line + "-");
        }
    }
}

void Octree::find_closest(const Point &target, int radius, Point &closestPoint, double &minDist) {
    for (const auto &p : points) {
        double dist = p.distance(target);
        if (dist < minDist && dist <= radius) {
            minDist = dist;
            closestPoint = p;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            children[i]->find_closest(target, radius, closestPoint, minDist);
        }
    }
}

Point Octree::find_closest(const Point &target, int radius) {
    Point closestPoint;
    double minDist = std::numeric_limits<double>::max();

    find_closest(target, radius, closestPoint, minDist);

    if (!closestPoint.init) {
        cout << "No se encontró un punto cercano." << endl;
    } else {
        cout << "Punto más cercano: " << closestPoint <<" with distance: "<<closestPoint.distance(target)<< endl;
    }
    return closestPoint;
}

void Octree::get_h_bottom(const Point &p){
    for(int i=0; i<points.size(); i++){
        if(p==points[i]){
            cout<<"bottomLeft: "<<bottomLeft<<" h: "<<h<<endl;
            return;
        }
    }

    for(int i=0; i<8; i++){
        if(children[i]!=nullptr)
            children[i]->get_h_bottom(p);
    }
}

void Octree::drawCube(const Point bottom, double h) {
    float x = bottom.x, y = bottom.y, z = bottom.z;
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);

    // Cara frontal
    glVertex3f(x, y, z);
    glVertex3f(x + h, y, z);
    glVertex3f(x + h, y + h, z);
    glVertex3f(x, y + h, z);

    // Cara trasera
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x, y, z + h);
    glVertex3f(x + h, y, z + h);
    glVertex3f(x + h, y + h, z + h);
    glVertex3f(x, y + h, z + h);

    // Cara izquierda
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + h);
    glVertex3f(x, y + h, z + h);
    glVertex3f(x, y + h, z);

    // Cara derecha
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x + h, y, z);
    glVertex3f(x + h, y, z + h);
    glVertex3f(x + h, y + h, z + h);
    glVertex3f(x + h, y + h, z);

    // Cara superior
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, y + h, z);
    glVertex3f(x + h, y + h, z);
    glVertex3f(x + h, y + h, z + h);
    glVertex3f(x, y + h, z + h);

    // Cara inferior
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x + h, y, z);
    glVertex3f(x + h, y, z + h);
    glVertex3f(x, y, z + h);

    glEnd();
}

void Octree::drawOctree() {
    bool leaf = true;
    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            leaf = false;
            break;
        }
    }

    if (leaf && !points.empty()) {
        drawCube(bottomLeft, h);
        //cout<<"bottomLeft: "<<bottomLeft<<" h: "<<h<<endl;
    } else {
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                children[i]->drawOctree();
            }
        }
    }
}
