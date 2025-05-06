#include <iostream>
#include <vector>

using namespace std;

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
};

struct KDNode {
    Point point;
    KDNode *left, *right;

    KDNode(Point p) : point(p), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    KDNode* insertRec(KDNode* root, Point point, unsigned depth) {
        if (root == nullptr) return new KDNode(point);

        unsigned cd = depth % 2;  // cd = current dimension (0: x, 1: y)

        if ((cd == 0 && point.x < root->point.x) || (cd == 1 && point.y < root->point.y))
            root->left = insertRec(root->left, point, depth + 1);
        else
            root->right = insertRec(root->right, point, depth + 1);

        return root;
    }

    bool arePointsEqual(Point p1, Point p2) {
        return p1.x == p2.x && p1.y == p2.y;
    }

    bool searchRec(KDNode* root, Point point, unsigned depth) {
        if (root == nullptr) return false;
        if (arePointsEqual(root->point, point)) return true;

        unsigned cd = depth % 2;

        if ((cd == 0 && point.x < root->point.x) || (cd == 1 && point.y < root->point.y))
            return searchRec(root->left, point, depth + 1);
        else
            return searchRec(root->right, point, depth + 1);
    }

    void inOrderRec(KDNode* root) {
        if (root != nullptr) {
            inOrderRec(root->left);
            cout << "(" << root->point.x << ", " << root->point.y << ") ";
            inOrderRec(root->right);
        }
    }

public:
    KDNode* root;

    KDTree() : root(nullptr) {}

    void insert(Point point) {
        root = insertRec(root, point, 0);
    }

    bool search(Point point) {
        return searchRec(root, point, 0);
    }

    void inOrder() {
        inOrderRec(root);
        cout << endl;
    }
};

int main() {
    KDTree tree;
    vector<Point> points = {
        {3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}
    };

    for (const auto& p : points) {
        tree.insert(p);
    }

    tree.inOrder();  // Recorrido simple

    Point searchPoint(10, 19);
    if (tree.search(searchPoint)) {
        cout << "Punto encontrado: (" << searchPoint.x << ", " << searchPoint.y << ")" << endl;
    } else {
        cout << "Punto NO encontrado." << endl;
    }

    return 0;
}