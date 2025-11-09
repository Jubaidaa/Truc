#include "point.h"
#include <iostream>

Point::Point(const double x, const double y): x(x),y(y){}

Point* Point::vector(Point* p){
  return new Point(p->x - x, p->y - y);
}

Point* Point::center(Point* b) {
  return new Point((x + b->x) / 2, (y + b->y) / 2);
}

Point* Point::translate(Point* p){
  return new Point(x + p->x, y + p->y);
}

Point* Point::scal(double coef){
  return new Point(x * coef, y * coef);
}

double Point::det(Point* p){
  return x * p->y - y * p->x;
}

std::string Point::to_string (){
  return std::to_string(x) + " " + std::to_string(y);
}
