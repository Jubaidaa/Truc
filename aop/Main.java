import java.util.Random;
import java.util.ArrayList;

public class Main{

  static int pass = 0;
  static int fail = 0;

  static <T> void test (String name, T actual, T expected){
    System.out.print("Testing "+name+":");
    if (actual!=null) {
      if (expected instanceof String)  {
        if (actual.toString().equals(expected)){
          pass++;
          System.out.println(" PASS");
        } else {
          fail++;
          System.out.println( " FAIL: '" + actual + "' instead of '" + expected + "'");
        }
      } else if (actual.equals(expected)) {
        pass++;
        System.out.println( " PASS");
      } else {
        fail++;
        System.out.println( " FAIL: '" + actual + "' instead of '" + expected + "'");
      }
    }else {
        fail++;
        System.out.println( " FAIL: null instead of '" + expected + "'");
      }
  }

  public static void main(String[] args){

    int nb=100;

    Point p1 = new Point (0,0);
    Point p2 = new Point (0,1);
    Point p3 = new Point (1,1);
    Point p4 = new Point (1,0);
    Point p5 = new Point (5,50);
    Point p6 = new Point (50,5);

    Rectangle r0 = new Rectangle (p1, 10, 100);
    Rectangle r1 = new Rectangle (p1, 0, 0);

    Ellipse e = new Ellipse(p1, 2.0, 1.0);
    Ellipse e2 = new Ellipse(p1, 0, 0);

    Random gen = new Random();

    // ---------- Rectangle tests ----------
    ////////////////////////////////////////

    boolean bb_check =
      r0.boundingBox() != null &&
      r0.boundingBox().minX() == 0 &&
      r0.boundingBox().maxX() ==  10 &&
      r0.boundingBox().minY() == 0 &&
      r0.boundingBox().maxY() ==  100;

    test ("rectangle bb", bb_check, true);
    test ("rectangle mem inside", r0.belongs(p5), true);
    test ("rectangle mem outside", r0.belongs(p6), false);
    test ("rectangle random point", r1.random(gen,nb), "0.0 0.0");

    Point pp;
    int in_r = 0;
    // random sampling: sampled point must belong to the ellipse
     for (int i=0; i<100; i++){
       pp=r0.random(gen,nb);
      if(pp != null && r0.belongs(pp)) in_r++;
    }
    test ("rectangle random", in_r, 100);

    // ---------- Ellipse tests ----------
    //////////////////////////////////////

    bb_check =
      e.boundingBox() != null &&
      e.boundingBox().minX() == -2.0 &&
      e.boundingBox().maxX() ==  2.0 &&
      e.boundingBox().minY() == -1.0 &&
      e.boundingBox().maxY() ==  1.0;
    test ("ellipse bb", bb_check, true);

    test ("ellipse mem inside",  e.belongs(new Point(1.0, 0.0)), true);
    test ("ellipse mem outside", e.belongs(new Point(3.0, 0.0)), false);

    test ("ellipse random point", e2.random(gen,nb), "0.0 0.0");

    int in_e = 0;
     for (int i=0; i<100; i++){
       pp=e.random(gen,nb);
      if(pp != null && e.belongs(pp)) in_e++;
    }
    test ("ellipse random", in_e, 100);

    // ---------- Union tests ----------
    ////////////////////////////////////
    ArrayList<Shape> list = new ArrayList<Shape>();
    list.add(r0);
    list.add(e);
    Union u = new Union(list);

    Rectangle ub = u.boundingBox();
    bb_check =
      ub != null &&
      ub.minX() == -2.0 &&
      ub.minY() == -1.0 &&
      ub.maxX() == 10.0 &&
      ub.maxY() == 100.0;
    test("union bb", bb_check, true);

    test ("union mem inside", u.belongs(new Point(1.0, 0.0)), true);
    test ("union mem outside", u.belongs(new Point(1000.0, 1000.0)), false);


    list = new ArrayList<Shape>();
    list.add(r0);
    list.add(e2);
    Union u2 = new Union(list);

    test ("union random point", e2.random(gen,nb), "0.0 0.0");

    int in_u = 0;
     for (int i=0; i<100; i++){
       pp=u.random(gen,nb);
      if(pp != null && u.belongs(pp)) in_u++;
    }
    test ("union random", in_u, 100);

    System.out.println(pass + "/" + (pass+fail));
  }
}
