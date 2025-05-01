#include <string>
#include <vector>
#include <iostream>
#include <random>
#include "RTree.h"

using namespace std;

vector<vector<pair<int, int>>> generarPuntosRandom(int n, int k = 2, int rango_min = -100, int rango_max = 100) {
  vector<vector<pair<int, int>>> conjuntos;
  // std::ofstream archivoCSV("puntos.csv");
    
  // if (!archivoCSV.is_open()) {
  //     std::cerr << "Error al abrir el archivo " << std::endl;
  //     return conjuntos;
  // }
  // archivoCSV << "x,y" << std::endl;
  // uniform_int_distribution<> dis(rango_min, rango_max);
  random_device rd;
  mt19937 gen(rd());

  for (int i = 0; i < n; ++i) {
      uniform_int_distribution<> dis(rango_min, rango_max);
      rango_min++;
      if(rango_min+10>=rango_max){
        rango_max+=100;
      }
      vector<pair<int, int>> conjunto;
      for (int j = 0; j < k; ++j) {
          int x = dis(gen);
          int y = dis(gen);
          // archivoCSV << x << "," << y << std::endl;
          // cout<<x<<","<<y<<endl;
          conjunto.emplace_back(x, y);
      }
      conjuntos.push_back(conjunto);
  }

  // archivoCSV.close();

  // if (!archivoCSV) {
  //   std::cerr << "Error al cerrar el archivo "<< std::endl;
  // } else {
  //     cout << "Archivo guardado correctamente " << endl;
  // }
  return conjuntos;
}

 void print(const int& command, vector<vector<vector<pair<int, int>>>>& objects_n, string& output) {
	output.resize(0);
	output = command;

	switch (command)
	{
    case 0:// OBJECTS
		output += "|" + to_string(objects_n[0].size());
		for (auto& polygon : objects_n[0]) {
			output += "|" + to_string(polygon.size());
      
			for (auto& point : polygon) {
				output += "|" + to_string(point.first) + "|" + to_string(point.second);
			}
		}
		break;

	case 1: //MBRS:
    cout<<" number of (objects) :"<< to_string(objects_n.size())<<endl;
		output += "|" + to_string(objects_n.size());
		for (auto& objects : objects_n) {
			output += "|" + to_string(objects.size());
      cout<<" number of polygons :"<< to_string(objects.size())<<endl;
			for (auto& polygon : objects) {
				output += "|" + to_string(polygon.size());
        cout<<" number of points :"<< to_string(polygon.size())<<endl;
				for (auto& point : polygon) {
					output += "|" + to_string(point.first) + "|" + to_string(point.second);
          cout<<"   point:"<< to_string(point.first) << " | " << to_string(point.second)<<endl;
          
				}
			}
      cout<<endl<<"--------------------"<< endl; 
		}
		break;
	default:
		output += "|0";
		break;
	}

	output += "|END";
  //cout<<output;
}

void print_pair(vector<pair<int, int>> output){
  for(auto &x:output)
  {
    cout<<" ( "<<x.first<<" , "<<x.second<< " ) ";
  }
  }

int main(int argc, char *argv[])
{
	vector<vector<pair<int, int>>> vpoints = generarPuntosRandom(1000,10);
  
    //   First Test
  float coord[16] = {20, 59, 20, 43, 50, 58 , 48,67, 105, 68, 74, 64, 83, 40, 104, 54};
  
  vector<pair<int, int>> points;
  for(int i =0;i<16;i+=2){
    pair<int,int> A;
    A.first = coord[i];
    A.second = coord[i+1];
    points.push_back(A);          
  }
  for (unsigned int i =0;i<8;i+=2){       
    vector<pair<int, int>>  sub1(&points[i],&points[i+2]);
    vpoints.push_back(sub1);
  
  }


  float coord2[12] = {12, 28, 19,15, 40,29, 69,25, 70,28, 60,15}; // 54,12};
  
  vector<pair<int, int>> points2;
  for(int i =0;i<12;i+=2){
    pair<int,int> A;
    A.first = coord2[i];
    A.second = coord2[i+1];
    points2.push_back(A);          
  }
  for (unsigned int i =0;i<6;i+=3){  
    vector<pair<int, int>>  sub1(&points2[i],&points2[i+3]);
    vpoints.push_back(sub1);
  
  }


  RTree rtree;

	string output;
	vector<vector<pair<int, int>>> objects;

  vector<vector<vector<pair<int, int>>>> objects_n;

  for(auto &x:vpoints)
    {
      cout<<"inserting "<< x.size()<< ": ";
      print_pair(x);
      Rect rect = rtree.MBR(x);
      rtree.Insert(rect.m_min, rect.m_max, x);
      cout<< endl;
    }  
 //Rect rect = rtree.MBR(vpoints[0]);
  //rtree.Insert(rect.m_min, rect.m_max, vpoints[0]);
  rtree.getMBRs(objects_n);
  print(1, objects_n, output);


  //   Second Test
  // Adding one more region to get a bigger tree
  cout<< " INSERTING ONE MORE "<< endl;
  vector<pair<int, int>> ad;
  pair<int, int> ad1;
  ad1.first=54;
  ad1.second=12;
  pair<int, int> ad2;
  ad2.first=53;
  ad2.second=4;
  ad.push_back(ad2);
  ad.push_back(ad1);
  
  cout<<"inserting "<< ad.size()<< ": ";
  print_pair(ad);
  Rect rect = rtree.MBR(ad);
  rtree.Insert(rect.m_min, rect.m_max, ad);
  cout<< endl;

 //Rect rect = rtree.MBR(vpoints[0]);
  //rtree.Insert(rect.m_min, rect.m_max, vpoints[0]);
  rtree.getMBRs(objects_n);
  print(1, objects_n, output);



  cout<< " REMOVING LAST INSERTED "<< endl;
  print_pair(ad);
  rtree.Remove(rect.m_min, rect.m_max, ad);
  cout<< endl;

 //Rect rect = rtree.MBR(vpoints[0]);
  //rtree.Insert(rect.m_min, rect.m_max, vpoints[0]);
  rtree.getMBRs(objects_n);
  print(1, objects_n, output);


  Rect searchRect(0, 0, 100, 100);

  std::vector<std::pair<int, int>> searchResults = rtree.Search(searchRect);

  std::cout << "Resultados de la búsqueda (" << searchResults.size() << " puntos encontrados):" << std::endl;
  if (searchResults.empty()) {
      std::cout << "No se encontraron puntos en el área de búsqueda." << std::endl;
  } else {
      for (const auto& result : searchResults) {
          std::cout << "Punto: (" << result.first << ", " << result.second << ")" << std::endl;
      }
  }
  
  rtree.guardarMBRsEnCSV("mbrs_rtree.csv");
  rtree.ExportTreeToCSV();

	return 0;
}