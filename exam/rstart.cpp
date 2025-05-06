#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <iostream>

// Rectángulo
class Rect {
public:
    float x_min, x_max, y_min, y_max;
    // Área de un rectángulo
    float area() const {
        return (x_max - x_min) * (y_max - y_min);
    }
    // Centro del rectángulo
    std::pair<float, float> center() const {
        return { (x_min + x_max) / 2, (y_min + y_max) / 2 };
    }
};

// Nodo del R*-Tree
class RStarNode {
public:
    std::vector<Rect> data;
    int level;  // Nivel del nodo (0 = hoja)
    // Agregar otros campos si es necesario (punteros a hijos, etc.)
};

class RStar {
private:
    static const int MAX_ENTRIES = 8; // Capacidad máxima del nodo
    static const float REINSERT_PCT; // Porcentaje para reinserción
    std::map<int, int> reinsertions_per_level; // Control de reinserciones por nivel

public:
    void OverflowTreatment(RStarNode* node);
    void ReInsert(RStarNode* node);
    void Split(RStarNode* node);

    // Funciones ya implementadas (según enunciado)
    RStarNode* ChooseSubTree(Rect input);
    void Delete(Rect a);
    RStarNode* Find(Rect a);
};

const float RStar::REINSERT_PCT = 0.3f; // 30%

// ===================== OVERFLOW TREATMENT ============================
void RStar::OverflowTreatment(RStarNode* node) {
    int level = node->level;

    // Solo una reinserción por nivel permitida
    if (reinsertions_per_level[level] == 0) {
        reinsertions_per_level[level] = 1;
        ReInsert(node);
    } else {
        // Ya se reinsertó antes en este nivel -> hacer split
        Split(node);
    }
}

// ========================= REINSERT ==================================
void RStar::ReInsert(RStarNode* node) {
    int n = node->data.size();
    int m = static_cast<int>(std::round(n * REINSERT_PCT)); // m elementos a reinsertar

    // Calcular centro del nodo
    float sum_x = 0, sum_y = 0;
    for (const auto& rect : node->data) {
        auto [cx, cy] = rect.center();
        sum_x += cx;
        sum_y += cy;
    }
    float avg_x = sum_x / n, avg_y = sum_y / n;

    // Ordenar por distancia al centro promedio (mayor distancia primero)
    std::sort(node->data.begin(), node->data.end(), [avg_x, avg_y](const Rect& a, const Rect& b) {
        auto [ax, ay] = a.center();
        auto [bx, by] = b.center();
        float da = std::pow(ax - avg_x, 2) + std::pow(ay - avg_y, 2);
        float db = std::pow(bx - avg_x, 2) + std::pow(by - avg_y, 2);
        return da > db; // Mayor distancia primero
    });

    // Extraer m elementos más lejanos
    std::vector<Rect> to_reinsert(node->data.begin(), node->data.begin() + m);
    node->data.erase(node->data.begin(), node->data.begin() + m);

    // Reinsertarlos uno por uno
    for (const Rect& rect : to_reinsert) {
        RStarNode* destino = ChooseSubTree(rect);
        destino->data.push_back(rect);
        if (destino->data.size() > MAX_ENTRIES) {
            OverflowTreatment(destino); // Recursivo
        }
    }
}

// ============================ SPLIT ===================================
void RStar::Split(RStarNode* node) {
    // Lógica simple: dividir por mitad (puedes mejorar usando heurísticas R*-Tree reales)

    int n = node->data.size();
    std::sort(node->data.begin(), node->data.end(), [](const Rect& a, const Rect& b) {
        return a.x_min < b.x_min;
    });

    int mitad = n / 2;
    std::vector<Rect> group1(node->data.begin(), node->data.begin() + mitad);
    std::vector<Rect> group2(node->data.begin() + mitad, node->data.end());

    // Reemplazar nodo actual con uno de los grupos y crear nuevo nodo con el otro grupo
    node->data = group1;

    RStarNode* nuevo = new RStarNode();
    nuevo->level = node->level;
    nuevo->data = group2;

    // Aquí deberías insertar el nuevo nodo en el padre, expandir el árbol, etc.
    // Este paso depende del diseño del árbol, por lo que lo dejamos como un comentario.

    // Por ejemplo:
    // InsertIntoParent(node, nuevo);
}

