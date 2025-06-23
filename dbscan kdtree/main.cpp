#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;

struct Point {
    double x, y;
    int cluster = 0; // 0 sin asignar, -1 ruido, >0 cluster
    bool visited = false;
};

// ---------------- Utils ------------------------

double distance(const Point& a, const Point& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

vector<Point> readCSV(const string& filename) {
    vector<Point> points;
    ifstream file(filename);
    string line;
    getline(file, line); // saltar cabecera
    while (getline(file, line)) {
        stringstream ss(line);
        string xStr, yStr;
        getline(ss, xStr, ',');
        getline(ss, yStr, ',');
        Point p;
        p.x = stod(xStr);
        p.y = stod(yStr);
        points.push_back(p);
    }
    return points;
}

void writeCSV(const string& filename, const vector<Point>& points) {
    ofstream out(filename);
    out << "x,y,cluster\n";
    for (const auto& p : points) {
        out << p.x << "," << p.y << "," << p.cluster << "\n";
    }
}

// ---------------- DBSCAN clásico ------------------

vector<int> regionQuery(const vector<Point>& points, int index, double eps) {
    vector<int> neighbors;
    for (int i = 0; i < points.size(); ++i)
        if (distance(points[index], points[i]) <= eps)
            neighbors.push_back(i);
    return neighbors;
}

void expandCluster(vector<Point>& points, int index, vector<int>& neighbors,
                   int clusterId, double eps, int minPts) {
    points[index].cluster = clusterId;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int nIdx = neighbors[i];
        if (!points[nIdx].visited) {
            points[nIdx].visited = true;
            auto moreNeighbors = regionQuery(points, nIdx, eps);
            if (moreNeighbors.size() >= minPts)
                neighbors.insert(neighbors.end(), moreNeighbors.begin(), moreNeighbors.end());
        }
        if (points[nIdx].cluster == 0 || points[nIdx].cluster == -1)
            points[nIdx].cluster = clusterId;
    }
}

void dbscan(vector<Point>& points, double eps, int minPts) {
    int clusterId = 0;
    for (int i = 0; i < points.size(); ++i) {
        if (points[i].visited) continue;
        points[i].visited = true;
        auto neighbors = regionQuery(points, i, eps);
        if (neighbors.size() < minPts) {
            points[i].cluster = -1;
        } else {
            ++clusterId;
            expandCluster(points, i, neighbors, clusterId, eps, minPts);
        }
    }
}

// ---------------- KDTree ------------------

struct KDNode {
    Point point;
    KDNode *left = nullptr, *right = nullptr;
    bool vertical;
    KDNode(Point p, bool v) : point(p), vertical(v) {}
};

class KDTree {
public:
    KDTree(const vector<Point>& pts) {
        points = pts;
        root = build(0, points.size(), true);
    }

    vector<int> pointsInSphere(const Point& q, double eps) const {
        vector<int> result;
        search(root, q, eps, result);
        return result;
    }

private:
    KDNode* root;
    vector<Point> points;

    KDNode* build(int l, int r, bool vertical) {
        if (l >= r) return nullptr;
        int m = (l + r) / 2;
        nth_element(points.begin() + l, points.begin() + m, points.begin() + r,
            [vertical](const Point& a, const Point& b) {
                return vertical ? a.x < b.x : a.y < b.y;
            });
        KDNode* node = new KDNode(points[m], vertical);
        node->left = build(l, m, !vertical);
        node->right = build(m + 1, r, !vertical);
        return node;
    }

    void search(KDNode* node, const Point& q, double eps, vector<int>& result) const {
        if (!node) return;
        if (distance(node->point, q) <= eps) {
            for (int i = 0; i < points.size(); ++i) {
                if (points[i].x == node->point.x && points[i].y == node->point.y) {
                    result.push_back(i);
                    break;
                }
            }
        }
        double delta = node->vertical ? (q.x - node->point.x) : (q.y - node->point.y);
        if (delta <= eps) search(node->left, q, eps, result);
        if (delta >= -eps) search(node->right, q, eps, result);
    }
};

// ---------------- DBSCAN KDTree ------------------

void expandCluster(vector<Point>& points, KDTree& tree, int index, vector<int>& neighbors,
                   int clusterId, double eps, int minPts) {
    points[index].cluster = clusterId;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int idx = neighbors[i];
        if (!points[idx].visited) {
            points[idx].visited = true;
            auto more = tree.pointsInSphere(points[idx], eps);
            if (more.size() >= minPts)
                neighbors.insert(neighbors.end(), more.begin(), more.end());
        }
        if (points[idx].cluster == 0 || points[idx].cluster == -1) {
            points[idx].cluster = clusterId;
        }
    }
}

void dbscanKdTree(vector<Point>& points, double eps, int minPts) {
    int clusterId = 0;
    KDTree tree(points);
    for (int i = 0; i < points.size(); ++i) {
        if (points[i].visited) continue;
        points[i].visited = true;
        auto neighbors = tree.pointsInSphere(points[i], eps);
        if (neighbors.size() < minPts) {
            points[i].cluster = -1;
        } else {
            ++clusterId;
            expandCluster(points, tree, i, neighbors, clusterId, eps, minPts);
        }
    }
}

// ---------------- MAIN ------------------
double estimarEps(const vector<Point>& points, int k = 4) {
    int n = points.size();
    vector<double> k_distances;

    for (int i = 0; i < n; ++i) {
        vector<double> dists;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            dists.push_back(distance(points[i], points[j]));
        }
        sort(dists.begin(), dists.end());
        k_distances.push_back(dists[k - 1]); // distancia al k-ésimo vecino
    }

    sort(k_distances.begin(), k_distances.end(), greater<>()); // orden descendente

    // Heurística simple: detectar el punto de máxima caída (mayor pendiente)
    double max_drop = 0.0;
    int best_index = 0;
    for (int i = 1; i < n; ++i) {
        double drop = k_distances[i - 1] - k_distances[i];
        if (drop > max_drop) {
            max_drop = drop;
            best_index = i;
        }
    }

    return k_distances[best_index]; // valor estimado de eps
}

double estimarEpsCodoMaximo(const vector<Point>& points, int k = 4) {
    int n = points.size();
    vector<double> k_distances;

    // Paso 1: calcular distancia al k-ésimo vecino más cercano
    for (int i = 0; i < n; ++i) {
        vector<double> dists;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            dists.push_back(distance(points[i], points[j]));
        }
        sort(dists.begin(), dists.end());
        k_distances.push_back(dists[k - 1]); // distancia al k-ésimo vecino
    }

    // Paso 2: ordenar en orden ascendente
    sort(k_distances.begin(), k_distances.end());

    // Paso 3: buscar el punto con mayor cambio (pendiente máxima)
    double max_diff = 0.0;
    int best_index = 0;
    for (int i = 1; i < n; ++i) {
        double diff = k_distances[i] - k_distances[i - 1];
        if (diff > max_diff) {
            max_diff = diff;
            best_index = i;
        }
    }

    // Paso 4: retornar el eps correspondiente al punto de mayor cambio
    return k_distances[best_index];
}

int estimarMinPts(int n_puntos, int dimensiones = 2) {
    int valor = static_cast<int>(round(log(n_puntos) * dimensiones));
    return std::max(3, valor); // mínimo razonable
}


int main() {
    vector<string> filenames = {
        "noisy_circles.csv", "noisy_moons.csv", "blobs.csv", "aniso.csv", "varied.csv"
    };

    double eps = 0.5;
    int minPts = 3;

    for (const auto& file : filenames) {
        auto pointsClassic = readCSV(file);
        eps = estimarEpsCodoMaximo(pointsClassic,minPts);
        minPts = estimarMinPts(pointsClassic.size());
        cout<<"mejor epsilom: "<<eps<<" mejor min pts: "<<minPts<<endl;
        cout << "Procesando: " << file << endl;

        // DBSCAN clásico
        
        auto start1 = high_resolution_clock::now();
        dbscan(pointsClassic, eps, minPts);
        auto end1 = high_resolution_clock::now();
        auto duration1 = duration_cast<milliseconds>(end1 - start1).count();
        string outputClassic = file.substr(0, file.find('.')) + "_dbscan.csv";
        writeCSV(outputClassic, pointsClassic);
        cout << "DBSCAN clásico: " << duration1 << " ms -> " << outputClassic << endl;

        // DBSCAN KDTree
        auto pointsKD = readCSV(file);
        auto start2 = high_resolution_clock::now();
        dbscanKdTree(pointsKD, eps, minPts);
        auto end2 = high_resolution_clock::now();
        auto duration2 = duration_cast<milliseconds>(end2 - start2).count();
        string outputKD = file.substr(0, file.find('.')) + "_kdtree.csv";
        writeCSV(outputKD, pointsKD);
        cout << "DBSCAN KDTree:  " << duration2 << " ms -> " << outputKD << endl;

        cout << "--------------------------------------------\n";
    }

    return 0;
}