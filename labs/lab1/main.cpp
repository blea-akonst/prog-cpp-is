#include <iostream>
#include "geometry.h"

int main()
{
    Point p(4, 5);
    p.print();

    std::vector<Point> array { Point (0,0), Point(0, 4), Point(3, 4), Point(3, 0), Point (0, 0) };

    Polyline polyline(array);
    polyline.print();

    ClosedPolyline closed(array);
    closed.print();

    Polygon polygon1(array);
    polygon1.print();

    Trapezoid trapezoid(array);
    trapezoid.print();

    return 0;
}