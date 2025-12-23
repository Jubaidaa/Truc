#include <iostream>
#include "constant.h"
#include "polka.h"

int pass = 0;
int fail = 0;

// Templated test function
template <typename T>
void test(const std::string& name, const T& actual, const T& expected) {
  if (actual == expected) {
    pass++;
    std::cout << name << " PASS" << std::endl;
  } else {
    fail++;
    std::cout << name << " FAIL: " << actual << " instead of " << expected << std::endl;
  }
}

// Specialization for pointer tests
void test_not_null(const std::string& name, Expression* actual) {
  if (actual != nullptr) {
    pass++;
    std::cout << name << " PASS" << std::endl;
  } else {
    fail++;
    std::cout << name << " FAIL: nullptr" << std::endl;
  }
}

int main() {
  Constant* c42 = new Constant(42);
  test("cast", cast_to_int(c42), 42);
  delete c42;

  // [42; 3]
  Polka p0;
  p0.push(3);
  test("push stack size 1", p0.size(), 1);
  p0.push(42);
  test("push stack size 2", p0.size(), 2);

  // [42; 3]
  int res0 = cast_to_int(p0.get_value());
  test("top of pile", res0, 42);

  // [(1,2), 42; 3]
  p0.push(new Constant(1), new Constant(2));
  test("push stack size pair", p0.size(), 3);

  // [(1,2),42; 3]
  Pair* respair0 = cast_to_pair(p0.get_value());
  test_not_null("top of pile pair", respair0);

  // [3; 42]
  Polka p1;
  p1.push(3);
  p1.push(42);
  p1.push("><");
  int res1 = cast_to_int(p1.get_value());
  test("swap top of pile", res1, 3);

  // (3 + 2) * 4
  Polka p2;
  p2.push(3);
  p2.push(2);
  p2.push("+");
  p2.push(4);
  p2.push("*");
  int res = cast_to_int(p2.get_value());
  test("(3+2) * 4", res, 20);

  // ((2 * 3) + 4) / (15 - ((2 * 3) * + 4))
  Polka p3;
  p3.push(2);
  p3.push(3);
  p3.push("*");
  p3.push(4);
  p3.push("+");
  p3.push(15);
  p3.push(2);
  p3.push(3);
  p3.push("*");
  p3.push(4);
  p3.push("+");
  p3.push("-");
  p3.push("/");
  int res3 = cast_to_int(p3.get_value());
  test("((2 * 3) + 4) / (15 - ((2 * 3) * + 4))", res3, 2);

  // fst (2,3)
  Polka p6;
  p6.push(3);
  p6.push(2);
  p6.push("@");
  test("pair size", p6.size(), 1);
  p6.push("$1");
  int res6 = cast_to_int(p6.get_value());
  test("$1 (2,3)", res6, 2);

  // snd (2,3)
  Polka p7;
  p7.push(new Constant(2), new Constant(3));
  p7.push("$2");
  int res7 = cast_to_int(p7.get_value());
  test("$2 (2,3)", res7, 3);

  std::string s = p0.to_string();
  std::string name = "to_string";
  std::string expected="[(1,2),42,3]";
  test(name, s, expected);
  test("to_string size", p0.size(), 3);

  // (3 : *)
  Polka p4;
  p4.push(3);
  p4.push(":");
  p4.push("*");
  int res4 = cast_to_int(p4.get_value());
  test("3 * 3", res4, 9);

  // ((2 * 3) * + 4) / (15 - ((2 * 3) * + 4))
  Polka p5;
  p5.push(2);
  p5.push(3);
  p5.push("*");
  p5.push(4);
  p5.push("+");
  p5.push(":");
  p5.push(15);
  p5.push("><");
  p5.push("-");
  p5.push("/");
  int res5 = cast_to_int(p5.get_value());
  test("((2 * 3) + 4) / (15 - ((2 * 3) * + 4))", res5, 2);

  // // test of copy
  // Polka p8;
  // Constant* mut = new Constant(42);
  // p8.push(mut);
  // p8.push(":");
  // int res8 = cast_to_int(p8.get_value());
  // mut->value=43;
  // int res82 = cast_to_int(p8.get_value());
  // test ("copy :", res82, 42);

  // // test of deep copy
  // Polka p9;
  // Constant* mut9 = new Constant(42);
  // Constant* z = new Constant(0);
  // Pair* p = new Pair(z, mut9);
  // p9.push(p);
  // p9.push(":");
  // mut9->value=43;
  // p9.push("$2");
  // int pres92fst = cast_to_int(p9.get_value());
  // test ("copy 2 :", pres92fst, 42);

  std::cout << pass << "/" << (pass+fail) << std::endl;
}
