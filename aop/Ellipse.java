import java.util.Random;

// Axis Aligned Ellipses
class Ellipse extends Shape {

  private Point center;
  private double rx, ry;

  public Ellipse(Point p, double rx, double ry) {
    center=p;
    this.rx = rx;
    this.ry = ry;
  }

  @Override
  public Rectangle boundingBox() {
    // replace the return null with your code!
    return null;
  }

  @Override
  public boolean belongs(Point p) {
    // replace the return null with your code!
    return true;
  }
}
