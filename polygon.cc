#include <cmath>
#include <vector>

#include "polygon.h"
#include "point.h"

Polygon::Polygon(std::vector<Point*> vert): vertices(vert){
  for(size_t i = 1; i + 1 < vertices.size(); i++){
    triangles.push_back(new Triangle(vertices[0], vertices[i], vertices[i + 1]));
  }
}

double Polygon::area(){
  double total = 0.0;
  for(size_t i = 0; i < triangles.size(); i++){
    total += triangles[i]->area();
  }
  return total;
}

Point* Polygon::random(){
  double total_area = area();
  double random_value = (rand() / (double)RAND_MAX) * total_area;
  double cumulative = 0.0;
  for(size_t i = 0; i < triangles.size(); i++){
    double tri_area = triangles[i]->area();
    cumulative += tri_area;
    if(random_value <= cumulative){
      return triangles[i]->random();
    }
  }
  return triangles[triangles.size() - 1]->random();
}
