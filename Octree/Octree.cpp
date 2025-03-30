#include "Octree.hpp"

bool Point::inside(const Point &p, int h) const{
    return p.x>=x && p.x<=x+h &&
            p.y>=y && p.y<=y+h &&
            p.z>=z && p.z<=z+h;
}

double  Point::distance(const Point &p) const{
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
}

Octree::Octree(const Point &p, double height, int capacity){
    bottomLeft = p;
    h = height;
    nPoints = capacity;

    for (int i = 0; i < 8; i++)
        children[i] = nullptr;
}

bool Octree::exist(const Point& p) {
    if (!points.size())
        return false;

    for(int i=0; i<points.size(); i++)
      if (points[i] == p) 
          return true;

    for (int i = 0; i < 8; i++)
        if (children[i] && children[i]->exist(p))
            return true;

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
        if(!children[0]){
            double mid_h = h/2;
            int z = bottomLeft.z, index=0;
            for(int i=0; i<2; i++){
                int y = bottomLeft.y;
                for(int j=0; j<2; j++){
                    int x = bottomLeft.x;
                    for(int k=0; k<2; k++){
                        children[index++] = new Octree(Point(x,y,z), mid_h, nPoints);
                        x+=mid_h;
                    }
                    y+=mid_h;
                }
                z+=mid_h;
            }
        }

        for(int i=0; i<8; i++)
            children[i]->insert(new_point);
    }
}

void Octree::printTree(std::string line) {
    if (points.size()) {
      std::cout << line;
      for(int i=0; i<points.size(); i++){
        std::cout<<points[i]<<" ";
      }
      std::cout<<std::endl;
    }
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            children[i]->printTree(line + "-");
        }
    }
}

// double Octree::childDistances(const Point &, int radius){

//     return ;
// }
Point Octree::find_closest(const Point &target, int radius) {
    Point closestPoint;
    double minDist = std::numeric_limits<double>::max();

    for (const auto &p : points) {
        double dist = p.distance(target);
        if (dist < minDist && dist <= radius) {
            minDist = dist;
            closestPoint = p;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (children[i] != nullptr) {
            Point candidate = children[i]->find_closest(target, radius);
            double childDist = candidate.distance(target);
            if (childDist <= minDist && candidate.init) {
                minDist = childDist;
                closestPoint = candidate;
            }
        }
    }
    return closestPoint;
}