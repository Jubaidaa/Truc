import java.util.ArrayList;
import java.util.Random;

class Union extends Shape {
    private ArrayList<Shape> shapes;

    public Union(ArrayList<Shape> shapes) {
        this.shapes = shapes;
    }

    @Override
    public Rectangle boundingBox() {
        if (shapes.isEmpty()) {
            return null;
        }

        double minX = Double.MAX_VALUE;
        double minY = Double.MAX_VALUE;
        double maxX = -Double.MAX_VALUE;
        double maxY = -Double.MAX_VALUE;

        for (Shape s : shapes) {
            Rectangle bb = s.boundingBox();
            if (bb != null) {
                minX = Math.min(minX, bb.minX());
                minY = Math.min(minY, bb.minY());
                maxX = Math.max(maxX, bb.maxX());
                maxY = Math.max(maxY, bb.maxY());
            }
        }

        Point bottomLeft = new Point(minX, minY);
        return new Rectangle(bottomLeft, maxX - minX, maxY - minY);
    }

    @Override
    public boolean belongs(Point p) {
        for (Shape s : shapes) {
            if (s.belongs(p)) {
                return true;
            }
        }
        return false;
    }
}