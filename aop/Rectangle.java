import java.util.Random;

class Rectangle extends Shape {
  private Point bottomLeft;
  private double width, height;

  public Rectangle(Point p, double width, double height) {
    this.bottomLeft=p;
    this.width = width;
    this.height = height;
  }

  public double minX() {
    return bottomLeft.x;
  }

  public double maxX() {
    return bottomLeft.x + width;
  }

  public double minY() {
    return bottomLeft.y;
  }

  public double maxY() {
    return bottomLeft.y + height;
  }

  @Override
  public Rectangle boundingBox() {
    // replace the return null with your code!
    return null;
  }

  @Override
  public boolean belongs(Point p) {
    // replace the return true with your code!
    return true;
  }
}
