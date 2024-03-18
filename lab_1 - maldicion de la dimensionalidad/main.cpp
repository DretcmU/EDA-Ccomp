#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());

std::vector<double> generarPuntoAleatorio(int dimension) {
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  std::vector<double> punto(dimension);

  for (int i = 0; i < dimension; i++)
    punto[i] = dis(gen);

  return punto;
}

double calcularDistanciaEuclidiana(const std::vector<double> &A, const std::vector<double> &B) {
  double distancia = 0.0;
  for (int i = 0; i < A.size(); i++)
    distancia += std::pow(A[i] - B[i], 2);

  return std::sqrt(distancia);
}

int main() {
  int n_puntos_aleatorios = 100;
  std::vector<int> dimensiones = {10, 50, 100, 500, 1000, 2000, 5000};

  for (int dimension : dimensiones) {
    std::vector<std::vector<double>> puntos;
    for (int i = 0; i < n_puntos_aleatorios; i++)
      puntos.push_back(generarPuntoAleatorio(dimension));

    std::ofstream archivo("distancias_" + std::to_string(dimension) + ".txt");
    if (!archivo.is_open()) {
      std::cerr << "Error al abrir el archivo." << std::endl;
      return 1;
    }

    int con=0;
    for (int i = 0; i < puntos.size(); i++) {
      for (int j = i + 1; j < puntos.size(); j++) {
        double distancia = calcularDistanciaEuclidiana(puntos[i], puntos[j]);
        archivo << distancia << " ";
        con++;
      }
    }

    archivo.close();

    std::cout << "Dimension: " << dimension << ", Distancias calculadas: " << con << std::endl;
  }

  return 0;
}
