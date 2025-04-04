#include "Octree.cpp"
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

float angleX = 30.0f, angleY = 30.0f, cameraZ = -1000.0f, cameraX=0.0f,cameraY=0.0f;
Octree *octree=nullptr;

void leerCSV(const std::string &archivo, std::vector<int> &columna1,
             std::vector<int> &columna2, std::vector<int> &columna3) {

  std::ifstream file(archivo);

  if (!file.is_open()) {
    std::cerr << "Error to open file." << std::endl;
    return;
  }

  std::string linea;
  while (std::getline(file, linea)) {
    std::istringstream ss(linea);
    std::string valor;
    std::getline(ss, valor, ',');
    int valor1 = std::stoi(valor);
    columna1.push_back(valor1);

    std::getline(ss, valor, ',');
    int valor2 = std::stoi(valor);
    columna2.push_back(valor2);

    std::getline(ss, valor, ',');
    int valor3 = std::stoi(valor);
    columna3.push_back(valor3);
  }

  file.close();
}

int max_num(int a,int b,int c){
  return a>b ? (a>c? a:c) : (b>c? b:c);
}

int get_h(vector<int> X, vector<int> Y, vector<int> Z, Point &p_aux) {
  p_aux = Point(
    *std::min_element(X.begin(),X.end()),
    *std::min_element(Y.begin(),Y.end()),
    *std::min_element(Z.begin(),Z.end())
  );

  int max_x = *std::max_element(X.begin(),X.end()), 
  max_y = *std::max_element(Y.begin(),Y.end()), 
  max_z = *std::max_element(Z.begin(),Z.end());
  
  return max_num(
    max_x >= 0 ? max_x - p_aux.x : max_x - p_aux.x, 
    max_y >= 0 ? max_y - p_aux.y : max_x - p_aux.y, 
    max_z >= 0 ? max_z - p_aux.z : max_x - p_aux.z
  );
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glTranslatef(cameraX, cameraY, cameraZ);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(octree!=nullptr)
      octree->drawOctree();

    glutSwapBuffers();
}

void keyboard(int key, int, int) {
    if (key == GLUT_KEY_LEFT)  angleY -= 5.0f;
    if (key == GLUT_KEY_RIGHT) angleY += 5.0f;
    if (key == GLUT_KEY_UP)    angleX -= 5.0f;
    if (key == GLUT_KEY_DOWN)  angleX += 5.0f;
    if (key == GLUT_KEY_PAGE_UP){
      cameraZ += 10.0f; // 🔹 Acerca la cámara
      //cout<<cameraZ<<endl;
    }
    if (key == GLUT_KEY_PAGE_DOWN){
      cameraZ -= 10.0f; // 🔹 Aleja la cámara
      //cout<<cameraZ<<endl;
    }
    glutPostRedisplay();
}

void keyboard2(unsigned char key, int, int) {
    if (key == 'w')
      cameraY += 10.0f;
    if (key == 's')
      cameraY -= 10.0f;
    if (key == 'a')
      cameraX += 10.0f;
    if (key == 'd')
      cameraX -= 10.0f;
    glutPostRedisplay();
}

void initLighting() {
    glEnable(GL_LIGHTING);        // Activar la iluminación
    glEnable(GL_LIGHT0);          // Activar la fuente de luz 0

    // Configurar la luz
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // Dirección de la luz (x, y, z)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // Color de la luz (blanca)
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };   // Luz ambiente (más suave)

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // Posición de la luz
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);    // Color de la luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);    // Luz ambiente

    glEnable(GL_COLOR_MATERIAL);  // Para que los objetos reflejen la luz correctamente
    glColorMaterial(GL_FRONT, GL_DIFFUSE);  // Usar el color difuso de los materiales
}



void initGL() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.5, 1.0, 4000.0);
    glMatrixMode(GL_MODELVIEW);

    initLighting();
}


int main(int argc, char** argv) {
  // vector<int> X({9,12,3,-10,5,11,13,14,-14,-13,-11}),
  // Y({3,6,8,7,4,9,12,3,-9,11,6}), Z({10,7,5,1,9,3,9,15,-12,2,8});
  std::vector<int> X;
  std::vector<int> Y;
  std::vector<int> Z;
  int N;

  leerCSV("points2.csv", X, Y, Z);

  Point p_aux;
  int h = get_h(X, Y, Z, p_aux);

  cout<<"BottomLeft: "<<p_aux<<" h: "<<h<<endl;
  cout<<"Enter N points: "; cin>>N;

  octree = new Octree(p_aux, h, N);

  for (int i = 0; i < X.size(); i++) {
    octree->insert(Point(X[i], Y[i], Z[i]));
  }

  std::cout << "Estructura del Octree:" << std::endl;
  octree->printTree();

  Point p_2find=Point(0,0,0);
  int radius=100;
  cout<<"Point exist in the tree? ";
  cout<<octree->exist(p_2find)<<endl;

  std::cout << "Find closest del Octree:" << std::endl;
  Point found = octree->find_closest(p_2find, radius);
  octree->get_h_bottom(found);

  cout<<"OCTREE BottomLeft: "<<p_aux<<" and h: "<<h<<endl;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1200, 800);
  glutCreateWindow("Octree with GLUT");

  initGL();


  glutDisplayFunc(display);
  glutSpecialFunc(keyboard);
  glutKeyboardFunc(keyboard2);

  glutMainLoop();

  while (1){
    cout<<" enter new point: ";
    int a,b,c, radius;
    cin>>a;
    cin>>b;
    cin>>c;
    Point p_find=Point(a,b,c);
    cin>>radius;
    cout<<"Point exist in the tree? ";
    cout<<octree->exist(p_find)<<endl;

    std::cout << "\nFind closest del Octree:" << std::endl;
    Point found2 = octree->find_closest(p_find, radius);
    octree->get_h_bottom(found2);
  }

  delete octree;
  return 0;
};
