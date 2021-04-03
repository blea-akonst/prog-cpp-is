#include <iostream>
#include "geometry.h"

int main()
{
    Point p(4, 5);
    p.print();

    std::vector<Point> array { Point (0,0), Point(0, 4), Point(3, 4), Point(3, 0), Point (0, 0) };

    Polyline polyline(array);
    polyline.printLine();

    ClosedPolyline closed(array);
    closed.printLine();

    Polygon polygon1(array);
    polygon1.printPolygon();

    Trapezoid trapezoid(array);
    trapezoid.printPolygon();

    return 0;
}