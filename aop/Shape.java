import java.util.Random;

abstract class Shape {
    public abstract Rectangle boundingBox();
    public abstract boolean belongs(Point p);

    public Point random(Random gen, int nb) {
        Rectangle bb = boundingBox();
        if (bb == null) {
            return null;
        }
        
        for (int i = 0; i < nb; i++) {
            double x = bb.minX() + gen.nextDouble() * (bb.maxX() - bb.minX());
            double y = bb.minY() + gen.nextDouble() * (bb.maxY() - bb.minY());
            Point p = new Point(x, y);
            if (belongs(p)) {
                return p;
            }
        }
        return null;
    }
}