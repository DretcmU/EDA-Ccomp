#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> countRectangles(vector<vector<int>>& rectangles, vector<vector<int>>& points) {
    vector<vector<int>> heightBuckets(101); // height -> list of lengths

    // Clasificamos longitudes por altura
    for (auto& rect : rectangles) {
        int l = rect[0], h = rect[1];
        heightBuckets[h].push_back(l);
    }

    // Ordenamos los buckets para aplicar búsqueda binaria
    for (int h = 1; h <= 100; ++h) {
        sort(heightBuckets[h].begin(), heightBuckets[h].end());
    }

    vector<int> result;
    for (auto& point : points) {
        int x = point[0], y = point[1];
        int count = 0;

        // Desde la altura y del punto hasta 100
        for (int h = y; h <= 100; ++h) {
            const auto& lengths = heightBuckets[h];
            // Buscamos cuántos rectángulos tienen l >= x
            count += lengths.end() - lower_bound(lengths.begin(), lengths.end(), x);
        }

        result.push_back(count);
    }

    return result;
}

void printVector(const vector<int>& v) {
    cout << '[';
    for (int i = 0; i < v.size(); ++i) {
        cout << v[i];
        if (i != v.size() - 1) cout << ", ";
    }
    cout << ']' << endl;
}

int main() {
    vector<vector<int>> rectangles1 = {{1, 2}, {2, 3}, {2, 5}};
    vector<vector<int>> points1 = {{2, 1}, {1, 4}};
    vector<int> result1 = countRectangles(rectangles1, points1);
    cout << "Output para el Ejemplo 1: ";
    printVector(result1); // Output esperado: [2, 1]

    vector<vector<int>> rectangles2 = {{1, 1}, {2, 2}, {3, 3}};
    vector<vector<int>> points2 = {{1, 3}, {1, 1}};
    vector<int> result2 = countRectangles(rectangles2, points2);
    cout << "Output para el Ejemplo 2: ";
    printVector(result2); // Output esperado: [1, 3]

    return 0;
}
