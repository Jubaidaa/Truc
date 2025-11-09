#ifndef POINT_H
#define POINT_H

#include <iostream>

class Point{
public:
  double x,y;

  Point(const double x, const double y);

  Point* vector(Point* p);
  Point* center(Point* p);
  Point* translate(Point* p);
  Point* scal(double coef);
  double det(Point* p);
  std::string to_string ();
};
#endif
