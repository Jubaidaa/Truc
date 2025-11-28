import java.util.Random;

class Rectangle extends Shape {
    private Point bottomLeft;
    private double width, height;

    public Rectangle(Point p, double width, double height) {
        this.bottomLeft = p;
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
        return this;
    }

    @Override
    public boolean belongs(Point p) {
        return p.x >= minX() && p.x <= maxX() && p.y >= minY() && p.y <= maxY();
    }
}