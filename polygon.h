#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "point.h"
#include "triangle.h"

class Polygon{
public:
  std::vector<Point*> vertices;
  std::vector<Triangle*> triangles;

  Polygon(std::vector<Point*> vert);
  double area();
  Point* random();
};
#endif
