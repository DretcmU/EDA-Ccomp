#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <chrono>

using namespace std;

struct Punto {
    float x, y;
    int indice;

    bool operator==(const Punto& otro) const {
        return x == otro.x && y == otro.y;
    }
};

float distancia(const Punto& a, const Punto& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

struct NodoKD {
    Punto punto;
    unique_ptr<NodoKD> izquierda;
    unique_ptr<NodoKD> derecha;

    NodoKD(Punto p) : punto(p), izquierda(nullptr), derecha(nullptr) {}
};

class KDTree {
private:
    unique_ptr<NodoKD> raiz;

    void insertarRec(unique_ptr<NodoKD>& nodo, const Punto& punto, int profundidad) {
        if (!nodo) {
            nodo = make_unique<NodoKD>(punto);
            return;
        }

        int eje = profundidad % 2;
        if ((eje == 0 && punto.x < nodo->punto.x) || (eje == 1 && punto.y < nodo->punto.y))
            insertarRec(nodo->izquierda, punto, profundidad + 1);
        else
            insertarRec(nodo->derecha, punto, profundidad + 1);
    }

    void nearestNeighborRec(NodoKD* nodo, const Punto& objetivo, int profundidad,
                            Punto& mejor, double& mejorDistancia) const {
        if (!nodo) return;

        double d = distancia(objetivo, nodo->punto);
        if (d < mejorDistancia) {
            mejorDistancia = d;
            mejor = nodo->punto;
        }

        int eje = profundidad % 2;
        NodoKD* ladoPrimario = nullptr;
        NodoKD* ladoSecundario = nullptr;

        if ((eje == 0 && objetivo.x < nodo->punto.x) || (eje == 1 && objetivo.y < nodo->punto.y)) {
            ladoPrimario = nodo->izquierda.get();
            ladoSecundario = nodo->derecha.get();
        } else {
            ladoPrimario = nodo->derecha.get();
            ladoSecundario = nodo->izquierda.get();
        }

        nearestNeighborRec(ladoPrimario, objetivo, profundidad + 1, mejor, mejorDistancia);

        // Verifica si vale la pena buscar en el otro lado
        double distanciaEje = (eje == 0) ? abs(objetivo.x - nodo->punto.x) : abs(objetivo.y - nodo->punto.y);
        if (distanciaEje < mejorDistancia) {
            nearestNeighborRec(ladoSecundario, objetivo, profundidad + 1, mejor, mejorDistancia);
        }
    }

public:
    void insertar(const Punto& punto) {
        insertarRec(raiz, punto, 0);
    }

    int nearestNeighborIndex(const Punto& objetivo) const {
        if (!raiz) return -1;

        Punto mejor = raiz->punto;
        double mejorDistancia = distancia(objetivo, mejor);

        nearestNeighborRec(raiz.get(), objetivo, 0, mejor, mejorDistancia);

        return mejor.indice;
    }
};

pair<vector<Punto>, vector<int>> kmeans(vector<Punto>& puntos, int k, int max_iter = 100, bool guardar_csv = false, const string& nombre_csv = "evolucion_kdtree.csv") {
    int n = puntos.size();
    if (k > n) throw invalid_argument("k no puede ser mayor a la cantidad de puntos");

    random_device rd;
    mt19937 gen(rd());
    shuffle(puntos.begin(), puntos.end(), gen);

    vector<Punto> centroides;
    for (int i = 0; i < k; i++) {
        Punto c = puntos[i];
        c.indice = i;
        centroides.push_back(c);
    }

    vector<int> asignaciones(n, -1);
    vector<int> cambios_por_iteracion;

    for (int iter = 0; iter < max_iter; iter++) {
        KDTree kd;
        for (const auto& c : centroides) {
            kd.insertar(c);
        }

        bool hubo_cambio = false;

        for (int i = 0; i < n; i++) {
            int centroide_mas_cercano = kd.nearestNeighborIndex(puntos[i]);
            if (asignaciones[i] != centroide_mas_cercano) {
                asignaciones[i] = centroide_mas_cercano;
                hubo_cambio = true;
            }
        }

        if (!hubo_cambio) break;

        vector<Punto> nuevos_centroides(k, {0, 0, 0});
        vector<int> conteo(k, 0);

        for (int i = 0; i < n; i++) {
            int c = asignaciones[i];
            nuevos_centroides[c].x += puntos[i].x;
            nuevos_centroides[c].y += puntos[i].y;
            conteo[c]++;
        }

        int centroides_movidos = 0;
        for (int i = 0; i < k; i++) {
            if (conteo[i] > 0) {
                nuevos_centroides[i].x /= conteo[i];
                nuevos_centroides[i].y /= conteo[i];
            } else {
                nuevos_centroides[i] = centroides[i];
            }

            // Contar cambios de posición
            if (nuevos_centroides[i].x != centroides[i].x || nuevos_centroides[i].y != centroides[i].y) {
                centroides_movidos++;
            }

            nuevos_centroides[i].indice = i;
        }

        cambios_por_iteracion.push_back(centroides_movidos);
        centroides = nuevos_centroides;
    }

    if (guardar_csv) {
        ofstream file(nombre_csv);
        file << "iteracion,centroides_movidos\n";
        for (int i = 0; i < cambios_por_iteracion.size(); ++i) {
            file << i << "," << cambios_por_iteracion[i] << "\n";
        }
        file.close();
    }

    return {centroides, asignaciones};
}


vector<Punto> leerCSV(const string& nombreArchivo) {
    vector<Punto> puntos;
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombreArchivo << "\n";  // <--- Verifica apertura
        return puntos;
    }
    string linea;
    int indice = 0;

    getline(archivo, linea); // Salta encabezado

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string xStr, yStr;

        if (getline(ss, xStr, ',') && getline(ss, yStr, ',')) {
            Punto p;
            p.x = stof(xStr);  // Cambié a stof por ser float
            p.y = stof(yStr);
            p.indice = indice++;
            puntos.push_back(p);
        }
    }

    return puntos;
}



// struct Punto {
//     float x, y;
//     int indice = -1;
// };

// float distancia(const Punto& a, const Punto& b) {
//     float dx = a.x - b.x;
//     float dy = a.y - b.y;
//     return dx * dx + dy * dy; 
// }

pair<vector<Punto>, vector<int>> kmeans_fuerza_bruta(vector<Punto>& puntos, int k, int max_iter = 100, bool guardar_csv = false, const string& nombre_csv = "evolucion_centroides.csv") {
    int n = puntos.size();
    if (k > n) throw invalid_argument("k no puede ser mayor a la cantidad de puntos");

    random_device rd;
    mt19937 gen(rd());
    shuffle(puntos.begin(), puntos.end(), gen);

    vector<Punto> centroides;
    for (int i = 0; i < k; i++) {
        Punto c = puntos[i];
        c.indice = i;
        centroides.push_back(c);
    }

    vector<int> asignaciones(n, -1);
    vector<int> cambios_por_iteracion;

    for (int iter = 0; iter < max_iter; iter++) {
        bool hubo_cambio = false;

        // Paso 1: asignación de puntos al centroide más cercano
        for (int i = 0; i < n; i++) {
            float min_dist = numeric_limits<float>::max();
            int centroide_mas_cercano = -1;

            for (int j = 0; j < k; j++) {
                float d = distancia(puntos[i], centroides[j]);
                if (d < min_dist) {
                    min_dist = d;
                    centroide_mas_cercano = j;
                }
            }

            if (asignaciones[i] != centroide_mas_cercano) {
                asignaciones[i] = centroide_mas_cercano;
                hubo_cambio = true;
            }
        }

        if (!hubo_cambio) break;

        // Paso 2: actualizar centroides
        vector<Punto> nuevos_centroides(k, {0, 0, 0});
        vector<int> conteo(k, 0);

        for (int i = 0; i < n; i++) {
            int c = asignaciones[i];
            nuevos_centroides[c].x += puntos[i].x;
            nuevos_centroides[c].y += puntos[i].y;
            conteo[c]++;
        }

        int centroides_movidos = 0;
        for (int i = 0; i < k; i++) {
            if (conteo[i] > 0) {
                nuevos_centroides[i].x /= conteo[i];
                nuevos_centroides[i].y /= conteo[i];
            } else {
                nuevos_centroides[i] = centroides[i];
            }

            // contar si el centroide se movió
            if (nuevos_centroides[i].x != centroides[i].x || nuevos_centroides[i].y != centroides[i].y) {
                centroides_movidos++;
            }

            nuevos_centroides[i].indice = i;
        }

        cambios_por_iteracion.push_back(centroides_movidos);
        centroides = nuevos_centroides;
    }

    // Guardar CSV si se solicita
    if (guardar_csv) {
        ofstream file(nombre_csv);
        file << "iteracion,centroides_movidos\n";
        for (int i = 0; i < cambios_por_iteracion.size(); ++i) {
            file << i << "," << cambios_por_iteracion[i] << "\n";
        }
        file.close();
    }

    return {centroides, asignaciones};
}



void guardarTiempoCSV(const string& filename, const vector<tuple<int, int, double>>& tiempos) {
    ofstream out(filename);
    out << "n,k,tiempo_ms\n";
    for (const auto& [n, k, tiempo] : tiempos) {
        out << n << "," << k << "," << tiempo << "\n";
    }
}

int main() {
    vector<int> n_vals = {1000, 1150, 1300, 1450, 1600, 1750, 1900, 2050, 2200, 2400};
    vector<int> k_vals = {5, 15, 25, 50, 75};

    vector<Punto> puntos = leerCSV("points2d.csv");
    cout << "Total puntos leídos: " << puntos.size() << "\n";

    for(int i=1;i<11;i++){
        string title = "Test_" + to_string(i) + ".csv";
        auto resultado = kmeans(puntos, 18,100, true,title);
        title = "Test_force_" + to_string(i) + ".csv";
        auto resultado2 = kmeans_fuerza_bruta(puntos, 18,100,true,title);
    }

    vector<tuple<int, int, double>> tiempos;

    for (int k : k_vals) {

        for (int n : n_vals) {
            if (k >= n) continue;
            if (n > (int)puntos.size()) continue;
            vector<Punto> subpuntos(puntos.begin(), puntos.begin() + n);

            cout << "Ejecutando KMeans con n = " << n << ", k = " << k << "...\n";

            auto inicio = chrono::high_resolution_clock::now();
            auto resultado = kmeans(subpuntos, k);
            auto fin = chrono::high_resolution_clock::now();

            double tiempo_ms = chrono::duration<double, std::milli>(fin - inicio).count();
            tiempos.push_back({n, k, tiempo_ms});

            //string nombre_archivo = "resultado_n" + to_string(n) + "_k" + to_string(k) + ".csv";
        }
    }

    guardarTiempoCSV("tiempos.csv", tiempos);

    // Fuerza Bruta KMeans
    vector<tuple<int, int, double>> tiempos_fuerza_bruta;

    for (int k : k_vals) {
    
        for (int n : n_vals) {
            if (k >= n) continue;
            if (n > (int)puntos.size()) continue;
            vector<Punto> subpuntos(puntos.begin(), puntos.begin() + n);
            
            cout << "[FuerzaBruta] Ejecutando KMeans con n = " << n << ", k = " << k << "...\n";
    
            auto inicio = chrono::high_resolution_clock::now();
            auto resultado = kmeans_fuerza_bruta(subpuntos, k);
            auto fin = chrono::high_resolution_clock::now();
    
            double tiempo_ms = chrono::duration<double, std::milli>(fin - inicio).count();
            tiempos_fuerza_bruta.push_back({n, k, tiempo_ms});
        }
    }
    
    guardarTiempoCSV("fuerza_bruta.csv", tiempos_fuerza_bruta);
    
    return 0;
}