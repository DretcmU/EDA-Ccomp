#include <vector>
std::vector<int> v;

// Insertar
v.push_back(10);   // Añadir al final
v.emplace_back(20); // Más eficiente

// Acceder
int x = v[0];
int y = v.at(1);

// Eliminar
v.pop_back();      // Elimina el último
v.clear();         // Borra todo


#include <deque>
std::deque<int> d;

d.push_back(1);
d.push_front(2);
d.pop_back();
d.pop_front();

#include <list>
std::list<int> lst;

lst.push_back(1);
lst.push_front(2);
lst.pop_back();
lst.pop_front();

#include <array>
std::array<int, 5> arr = {1, 2, 3, 4, 5};

arr[0] = 10;


#include <forward_list>
std::forward_list<int> fl;

fl.push_front(5);
fl.insert_after(fl.begin(), 10);

#include <set>
std::set<int> s;

s.insert(3);
s.insert(1);
s.insert(2);

// Buscar
if (s.find(2) != s.end()) { /* existe */ }

#include <set>
std::multiset<int> ms;

ms.insert(2);
ms.insert(2);  // OK#include <map>
std::map<std::string, int> m;

m["uno"] = 1;
m.insert({"dos", 2});

// Buscar
if (m.count("uno")) { /* existe */ }

#include <map>
std::map<std::string, int> m;

m["uno"] = 1;
m.insert({"dos", 2});

// Buscar
if (m.count("uno")) { /* existe */ }

#include <unordered_set>
std::unordered_set<int> us;

us.insert(1);
us.insert(2);

#include <unordered_map>
std::unordered_map<std::string, int> um;

um["id"] = 100;

#include <stack>
std::stack<int> st;

st.push(5);
st.pop();
int top = st.top();

#include <queue>
std::queue<int> q;

q.push(10);
q.pop();
int front = q.front();

#include <queue>
std::priority_queue<int> pq;

pq.push(3);
pq.push(5);
pq.push(1);

int top = pq.top(); // El mayor
std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;

for (int x : contenedor) {
    std::cout << x << "\n";
}
También puedes usar iteradores:

cpp
Copiar
Editar
for (auto it = contenedor.begin(); it != contenedor.end(); ++it) {
    std::cout << *it << "\n";
}


std::map<std::string, int> mapa = {{"a",1}, {"b",2}};

for (const auto& par : mapa) {
    std::cout << par.first << ": " << par.second << "\n";
}

std::stack<int> st;
st.push(1); st.push(2); st.push(3);

while (!st.empty()) {
    std::cout << st.top() << "\n";
    st.pop();
}

#include <iostream>
#include <stack>

int main() {
    std::stack<int> original;
    original.push(10);
    original.push(20);
    original.push(30);

    // Copiar stack
    std::stack<int> copia = original;

    // Recorrer copia sin perder datos en el original
    std::cout << "Elementos del stack (de arriba a abajo):\n";
    while (!copia.empty()) {
        std::cout << copia.top() << "\n";
        copia.pop();
    }

    // Verificamos que el original sigue intacto
    std::cout << "\nTop del stack original: " << original.top() << "\n";

    return 0;
}
