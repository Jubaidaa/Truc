import java.util.Random;

class Ellipse extends Shape {

    private Point center;
    private double rx, ry;

    public Ellipse(Point p, double rx, double ry) {
        center = p;
        this.rx = rx;
        this.ry = ry;
    }

    @Override
    public Rectangle boundingBox() {
        Point bottomLeft = new Point(center.x - rx, center.y - ry);
        return new Rectangle(bottomLeft, 2 * rx, 2 * ry);
    }

    @Override
    public boolean belongs(Point p) {
        if (rx == 0 && ry == 0) {
            return p.x == center.x && p.y == center.y;
        }
        if (rx == 0) {
            return p.x == center.x && Math.abs(p.y - center.y) <= ry;
        }
        if (ry == 0) {
            return p.y == center.y && Math.abs(p.x - center.x) <= rx;
        }
        double dx = (p.x - center.x) / rx;
        double dy = (p.y - center.y) / ry;
        return dx * dx + dy * dy <= 1.0;
    }
}