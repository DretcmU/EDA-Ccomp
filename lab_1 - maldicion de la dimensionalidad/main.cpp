#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());

vector<double> generateRandomPoints(int dimension) {
  std::uniform_real_distribution<double> dis(0.0, 1.0);
  vector<double> point(dimension);

  for (int i = 0; i < dimension; i++)
    point[i] = dis(gen);

  return point;
}

double Euclidian(const vector<double> &A, const vector<double> &B) {
  double distance = 0.0;
  for (int i = 0; i < A.size(); i++)
    distance += std::pow(A[i] - B[i], 2);

  return std::sqrt(distance);
}

int main() {
  int n_points = 100;
  vector<int> dimensions = {10, 50, 100, 500, 1000, 2000, 5000};

  for (int dimension : dimensions) {
    vector<vector<double>> points;
    for (int i = 0; i < n_points; i++)
      points.push_back(generateRandomPoints(dimension));

    std::ofstream file_points(to_string(dimension) + ".txt");
    if (!file_points.is_open()) {
      std::cerr << "Error to create/open file." << endl;
      return 1;
    }

    for (int i = 0; i < n_points; i++)
      for (int j = i + 1; j < n_points; j++)
        file_points << Euclidian(points[i], points[j]) << endl;

    file_points.close();

    cout<<"Good in "<<dimension<<" dimention.\n";
  }

  return 0;
}
