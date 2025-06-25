#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include "Eigen/Dense"

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
    int index; // índice en el vector original
    KDNode* left = nullptr;
    KDNode* right = nullptr;
    bool vertical;

    KDNode(int idx, bool v) : index(idx), vertical(v) {}
};

class KDTree {
public:
    KDTree(const vector<Point>& pts) : points(pts) {
        indices.resize(points.size());
        for (int i = 0; i < indices.size(); ++i) indices[i] = i;
        root = build(0, points.size(), true);
    }

    vector<int> pointsInSphere(const Point& q, double eps) const {
        vector<int> result;
        search(root, q, eps, result);
        return result;
    }

private:
    const vector<Point>& points;
    vector<int> indices; // índices de los puntos
    KDNode* root;

    KDNode* build(int l, int r, bool vertical) {
        if (l >= r) return nullptr;
        int m = (l + r) / 2;
        auto comp = [&](int a, int b) {
            return vertical ? points[a].x < points[b].x : points[a].y < points[b].y;
        };
        nth_element(indices.begin() + l, indices.begin() + m, indices.begin() + r, comp);
        KDNode* node = new KDNode(indices[m], vertical);
        node->left = build(l, m, !vertical);
        node->right = build(m + 1, r, !vertical);
        return node;
    }

    void search(KDNode* node, const Point& q, double eps, vector<int>& result) const {
        if (!node) return;
        const Point& p = points[node->index];
        if (distance(p, q) <= eps)
            result.push_back(node->index);

        double delta = node->vertical ? (q.x - p.x) : (q.y - p.y);
        if (delta <= eps) search(node->left, q, eps, result);
        if (delta >= -eps) search(node->right, q, eps, result);
    }
};

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
    KDTree tree(points);
    int clusterId = 0;
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

// double estimarEps(const vector<Point>& points, int k = 4) {
//     int n = points.size();
//     vector<double> k_distances;

//     for (int i = 0; i < n; ++i) {
//         vector<double> dists;
//         for (int j = 0; j < n; ++j) {
//             if (i == j) continue;
//             dists.push_back(distance(points[i], points[j]));
//         }
//         sort(dists.begin(), dists.end());
//         k_distances.push_back(dists[k - 1]);
//     }

//     sort(k_distances.begin(), k_distances.end());

//     // Curvatura: distancia perpendicular al segmento entre extremos
//     double x1 = 0, y1 = k_distances.front();
//     double x2 = n - 1, y2 = k_distances.back();

//     double max_dist = -1.0;
//     int best_index = 0;
//     for (int i = 0; i < n; ++i) {
//         double xi = i, yi = k_distances[i];

//         double num = abs((y2 - y1) * xi - (x2 - x1) * yi + x2*y1 - y2*x1);
//         double den = sqrt((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1));
//         double dist = num / den;

//         if (dist > max_dist) {
//             max_dist = dist;
//             best_index = i;
//         }
//     }

//     return k_distances[best_index];
// }

// int estimarMinPts(const vector<Point>& points, int muestras = 100, double radio = -1.0) {
//     if (points.empty()) return 3;
//     int n = points.size();
//     muestras = min(muestras, n);

//     // Si no se especifica radio, usa una heurística (10% del diámetro)
//     if (radio < 0) {
//         double minx = points[0].x, maxx = points[0].x;
//         double miny = points[0].y, maxy = points[0].y;
//         for (const auto& p : points) {
//             minx = min(minx, p.x); maxx = max(maxx, p.x);
//             miny = min(miny, p.y); maxy = max(maxy, p.y);
//         }
//         double diam = sqrt((maxx - minx)*(maxx - minx) + (maxy - miny)*(maxy - miny));
//         radio = 0.1 * diam;
//     }

//     srand(time(nullptr));
//     int suma_vecinos = 0;
//     for (int i = 0; i < muestras; ++i) {
//         int idx = rand() % n;
//         int vecinos = 0;
//         for (int j = 0; j < n; ++j) {
//             if (i == j) continue;
//             if (distance(points[idx], points[j]) <= radio)
//                 vecinos++;
//         }
//         suma_vecinos += vecinos;
//     }

//     int promedio = suma_vecinos / muestras;
//     return max(3, promedio / 2); // más conservador
// }

// double estimarEps(const vector<Point>& points, int k = 4) {
//     int n = points.size();
//     vector<double> k_distances;

//     for (int i = 0; i < n; ++i) {
//         vector<double> dists;
//         for (int j = 0; j < n; ++j) {
//             if (i == j) continue;
//             dists.push_back(distance(points[i], points[j]));
//         }
//         sort(dists.begin(), dists.end());
//         k_distances.push_back(dists[k - 1]);
//     }

//     sort(k_distances.begin(), k_distances.end());

//     // Buscar el punto de máxima curvatura (codo)
//     double x1 = 0, y1 = k_distances.front();
//     double x2 = n - 1, y2 = k_distances.back();

//     double max_dist = -1.0;
//     int best_index = 0;
//     for (int i = 0; i < n; ++i) {
//         double xi = i, yi = k_distances[i];
//         double num = abs((y2 - y1) * xi - (x2 - x1) * yi + x2*y1 - y2*x1);
//         double den = sqrt((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1));
//         double dist = num / den;

//         if (dist > max_dist) {
//             max_dist = dist;
//             best_index = i;
//         }
//     }

//     double eps = k_distances[best_index];

//     // Analiza anisotropía del dataset (relación entre rangos x/y)
//     double minx = points[0].x, maxx = points[0].x;
//     double miny = points[0].y, maxy = points[0].y;
//     for (const auto& p : points) {
//         minx = min(minx, p.x); maxx = max(maxx, p.x);
//         miny = min(miny, p.y); maxy = max(maxy, p.y);
//     }

//     double rx = maxx - minx;
//     double ry = maxy - miny;
//     double ratio = rx > ry ? rx / ry : ry / rx;

//     if (ratio > 2.0) {
//         eps *= 1.3; // ajustar si hay estiramiento significativo
//     }

//     return eps;
// }


double estimarEpsCodoMaximo(const vector<Point>& points, int k = 4) {
    int n = points.size();
    vector<double> k_distances;

    for (int i = 0; i < n; ++i) {
        vector<double> dists;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            dists.push_back(distance(points[i], points[j]));
        }
        sort(dists.begin(), dists.end());
        k_distances.push_back(dists[k - 1]);
    }

    sort(k_distances.begin(), k_distances.end());

    double max_diff = 0.0;
    int best_index = 0;
    for (int i = 1; i < n; ++i) {
        double diff = k_distances[i] - k_distances[i - 1];
        if (diff > max_diff) {
            max_diff = diff;
            best_index = i;
        }
    }

    return k_distances[best_index];
}

double evaluarSeparacion(const vector<Point>& puntos, const vector<int>& clusters) {
    if (puntos.empty()) return 0.0;
    int n = puntos.size();
    double intra = 0, inter = 1e9;
    int total = 0;

    for (int i = 0; i < n; ++i) {
        if (clusters[i] <= 0) continue;
        for (int j = i + 1; j < n; ++j) {
            if (clusters[j] <= 0) continue;
            double d = distance(puntos[i], puntos[j]);
            if (clusters[i] == clusters[j])
                intra += d;
            else
                inter = min(inter, d);
            total++;
        }
    }
    if (total == 0) return 0;
    intra /= total;
    return inter / (intra + 1e-6); // mayor es mejor
}

double estimarEps(vector<Point> puntos, int minPts = 4) {
    int n = puntos.size();
    vector<double> k_dist;
    for (int i = 0; i < n; ++i) {
        vector<double> d;
        for (int j = 0; j < n; ++j)
            if (i != j) d.push_back(distance(puntos[i], puntos[j]));
        sort(d.begin(), d.end());
        k_dist.push_back(d[minPts - 1]);
    }
    sort(k_dist.begin(), k_dist.end());

    vector<double> percentiles = {0.60, 0.65, 0.70, 0.75, 0.78, 0.80, 0.82, 0.85, 0.88, 0.90};

    double bestScore = -1;
    double bestEps = 0;

    for (double perc : percentiles) {
        double eps = k_dist[int(n * perc)];
        for (auto& p : puntos) { p.cluster = 0; p.visited = false; }

        dbscanKdTree(puntos, eps, minPts); // tu función ya implementada

        vector<int> clusters;
        for (auto& p : puntos) clusters.push_back(p.cluster);

        double score = evaluarSeparacion(puntos, clusters);
        if (score > bestScore) {
            bestScore = score;
            bestEps = eps;
        }
    }

    return bestEps;
}

int estimarMinPts(const vector<Point>& puntos, double eps, int muestras = 200) {
    if (puntos.empty()) return 3;

    int n = puntos.size();
    muestras = min(muestras, n);

    // Estimar vecinos para cada punto aleatorio
    vector<int> vecinos;
    srand(time(nullptr));
    for (int i = 0; i < muestras; ++i) {
        int idx = rand() % n;
        int count = 0;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            if (distance(puntos[idx], puntos[j]) <= eps)
                count++;
        }
        vecinos.push_back(count);
    }

    // Quitar outliers extremos
    sort(vecinos.begin(), vecinos.end());
    int q1 = vecinos[muestras * 0.25];
    int q3 = vecinos[muestras * 0.75];
    double iqr = q3 - q1;
    vector<int> filtrados;

    for (int v : vecinos) {
        if (v >= q1 - 1.5 * iqr && v <= q3 + 1.5 * iqr)
            filtrados.push_back(v);
    }

    // Media de los vecinos filtrados
    double suma = 0;
    for (int v : filtrados) suma += v;
    int estimado = round(suma / filtrados.size());

    // Ajustes finales
    return max(4, estimado);
}


int detectarTipoDeDataset(const vector<Point>& puntos) {
    if (puntos.size() < 3) return 2;

    int n = puntos.size();
    Eigen::MatrixXd X(n, 2);
    for (int i = 0; i < n; ++i) {
        X(i, 0) = puntos[i].x;
        X(i, 1) = puntos[i].y;
    }

    // Centrar los datos
    Eigen::RowVector2d media = X.colwise().mean();
    Eigen::MatrixXd X_centered = X.rowwise() - media;

    // PCA: matriz de covarianza
    Eigen::Matrix2d cov = (X_centered.transpose() * X_centered) / double(n - 1);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> solver(cov);
    auto eigenvalues = solver.eigenvalues();

    double lambda1 = eigenvalues(1); // mayor
    double lambda2 = eigenvalues(0); // menor
    double var_ratio = lambda1 / (lambda1 + lambda2);

    // Bounding box ratio
    double minX = puntos[0].x, maxX = puntos[0].x;
    double minY = puntos[0].y, maxY = puntos[0].y;
    for (const auto& p : puntos) {
        minX = min(minX, p.x); maxX = max(maxX, p.x);
        minY = min(minY, p.y); maxY = max(maxY, p.y);
    }
    double bbox_ratio = max((maxX - minX) / (maxY - minY), (maxY - minY) / (maxX - minX));

    double elongacion = var_ratio * bbox_ratio;

    std::cout << "var_ratio = " << var_ratio << ", bbox_ratio = " << bbox_ratio << ", elongacion = " << elongacion << "\n";

    if (var_ratio > 0.85 || elongacion > 1.4)
        return 2;  // tipo más estructurado o denso

    return 1;  // tipo curvo, disperso o circular
}

pair<double, int> estimarParametrosAdaptativos(const vector<Point>& puntos) {
    int tipo = detectarTipoDeDataset(puntos);
    double eps;
    int minPts;

    if (tipo == 1) {
        eps = estimarEpsCodoMaximo(puntos, 4);
        minPts = 4;
    } else {
        minPts = estimarMinPts(puntos, 0.5);
        eps = estimarEps(puntos, minPts);
        minPts = estimarMinPts(puntos, eps);
    }
    
    cout<<"TIPO : "<<tipo<<endl;
    cout<<"mejor epsilom: "<<eps<<" mejor min pts: "<<minPts<<endl;
    return {eps, minPts};
}

int main() {
    vector<string> filenames = {
        "noisy_circles.csv", "noisy_moons.csv", "blobs.csv", "aniso.csv", "varied.csv"
    };

    double eps = 0.5;
    int minPts = 3;

    for (const auto& file : filenames) {
        auto pointsClassic = readCSV(file);
        pair<double, int> params = estimarParametrosAdaptativos(pointsClassic);
        cout << "Procesando: " << file << endl;
        double eps = params.first;
        int minPts = params.second;
        //eps = estimarEpsCodoMaximo(pointsClassic,minPts);
        //minPts = estimarMinPts(pointsClassic, eps);
        

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