#include "triangle.h"
#include "point.h"
#include <iostream>
#include "math.h"

Triangle::Triangle(Point* a, Point* b, Point* c): a(a), b(b), c(c) {}

double Triangle::area(){
  Point* ab = a->vector(b);
  Point* ac = a->vector(c);
  double det_value = ab->det(ac);
  double result = fabs(det_value) / 2.0;
  delete ab;
  delete ac;
  return result;
}

Point* Triangle::random(){
  double r1 = rand() / (double)RAND_MAX;
  double r2 = rand() / (double)RAND_MAX;
  Point* ab = a->vector(b);
  Point* ac = a->vector(c);
  Point* result;
  if(r1 + r2 < 1){
    Point* v1 = ab->scal(r1);
    Point* v2 = ac->scal(r2);
    Point* temp = a->translate(v1);
    result = temp->translate(v2);
    delete temp;
    delete v1;
    delete v2;
  }else{
    Point* v1 = ab->scal(1 - r1);
    Point* v2 = ac->scal(1 - r2);
    Point* temp = a->translate(v1);
    result = temp->translate(v2);
    delete temp;
    delete v1;
    delete v2;
  }
  delete ab;
  delete ac;
  return result;
}
