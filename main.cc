#include <iostream>
#include <vector>
#include "point.h"
#include "polygon.h"
#include "triangle.h"

using namespace std;

int pass=0;
int fail=0;

int test_int (string name, int actual, int expected){
  if (actual == expected) {
    pass++;
    cout << name << " PASS" << endl;
  } else {
    cout << name << " FAIL: " << actual << " instead of " << expected << endl;
    fail++;
  }
  return 0;
}

int test_string (const std::string& name, Point* actual, const std::string& expected){
  if (actual!=nullptr) {
    string actual_str = actual->to_string();
    if (actual_str == expected){
      pass++;
      cout << name << " PASS" << endl;
    } else {
      fail++;
      cout << name << " FAIL: '" << actual_str << "' instead of '" << expected << "'" << endl;
    }
  } else {
    fail++;
    cout << name << " FAIL: point nullptr instead of '" << expected << "'" << endl;
  }
  return 0;
}

int test_double (string name, double actual, double expected){
  if (actual == expected) {
    pass++;
    cout << name << " PASS" << endl;
  } else {
    cout << name << " FAIL: " << actual << " instead of " << expected << endl;
    fail++;
  }
  return 0;
}

int main() {
  Point* p1 = new Point (0,0);
  Point* p2 = new Point (0,1);
  Point* p3 = new Point (1,1);
  Point* p4 = new Point (1,0);
  Point* p5 = p2->translate(p2);
  Point* p6 = p2->scal(2);
  Point* v1_2 = p1->vector(p2);
  Point* mid1_2 = p1->center(p2);

  Triangle* t1 = new Triangle(p1,p2,p3);
  Triangle* t2 = new Triangle(p1,p5,p3);
  Triangle* t3 = new Triangle(p1,p1,p1);
  vector<Point*> pts1{p1, p2, p3, p4};
  Polygon* poly1 = new Polygon(pts1);
  vector<Point*> pts2{p1, p5, p3, p4};
  Polygon* poly2 = new Polygon(pts2);
  vector<Point*> pts3{p1, p1, p1};
  Polygon* poly3 = new Polygon(pts3);

  test_string ("point creation", p1, "0.000000 0.000000");
  test_string ("vector creation", v1_2, "0.000000 1.000000");
  test_string ("center", mid1_2, "0.000000 0.500000");
  test_string ("translation", p5, "0.000000 2.000000");
  test_string ("scal", p6, "0.000000 2.000000");
  test_double ("det 1", p1->det(p3), 0.0);
  test_double ("det 2", p4->det(p3), 1.0);
  test_double ("triangle volume 1", t1->area(), 0.5);
  test_double ("triangle volume 2", t2->area(), 1.);
  test_string ("triangle random", t3->random(), "0.000000 0.000000");
  test_double ("polygon volume 1", poly1->area(), 1.0);
  test_double ("polygon volume 2", poly2->area(), 1.5);
  test_string ("polygon random", poly3->random(), "0.000000 0.000000");
  test_double ("triangle volume 3", t3->area(), 0.);
  test_double ("polygon volume 3", poly3->area(), 0.);

  Triangle* t4 = new Triangle(new Point (0,0), new Point(1,1), new Point(1,0));
  int in_t = 0;
  Point* pp;
  for (int i = 0; i < 100; i++) {
    pp=t4->random();
    if(pp != nullptr && pp->x <= 1 && pp->y <= 1 && pp->x >= 0 && pp->y >= 0 && pp->x >= pp->y) in_t++;
  }
  test_int("Inside Triangle", in_t, 100);

  int in_p = 0;
  vector<Point*> pts4{new Point (0,0), new Point(0,1), new Point(1,1), new Point(1,0)};
  Polygon* poly4 = new Polygon(pts4);
  for (int i = 0; i < 100; i++) {
    pp = poly4->random();
    if(pp != nullptr && pp->x <= 1 && pp->y <= 1 && pp->x >= 0 && pp->y >= 0) in_p++;
  }
  test_int("Inside poly", in_p, 100);

  cout << pass << "/" << (pass+fail) << endl;

  return 0;
}
