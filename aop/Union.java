import java.util.ArrayList;
import java.util.Random;

class Union extends Shape {
  private ArrayList<Shape> shapes;

  public Union(ArrayList<Shape> shapes) {
    this.shapes=shapes;
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
