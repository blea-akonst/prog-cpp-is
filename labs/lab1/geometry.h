#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

class Point {
private:
    double x;
    double y;
public:
    explicit Point(double _x = 0, double _y = 0): // конструктор
            x(_x), y(_y) {};

    Point (const Point &point) = default; // конструктор копирования

    Point& operator= (const Point &point) = default; // перегрузка оператора присваивания

    double getX() const { return x; }
    double getY() const { return y; }

    void setXY(double _x, double _y) { x = _x; y = _y; }

    void print() const
    {
        std::cout << "Point's cords: " << "(" << getX() << "; " << getY() << ").\n\n";
    }
};

class Polyline
{
protected:
    std::vector<Point> vertices;
public:
    Polyline() = default;

    explicit Polyline(const std::vector<Point> &points)
    {
        for (Point p: points)
            vertices.push_back(p);
    }

    Polyline(const Point arr[], int size)
    {
        for (int i = 0 ; i < size; i++)
            vertices.push_back(arr[i]);
    }

    virtual Polyline& operator= (const std::vector<Point> &points)
    {
        vertices.assign(points.size(), Point (0, 0));
        for (Point u: points)
            vertices.push_back(u);

        return *this;
    }

    double vectorModule(const Point &a, const Point &b) const
    {
        double x = b.getX() - a.getX();
        double y = b.getY() - a.getY();
        return sqrt(x*x + y*y);
    }

    virtual double getLength() const
    {
        double length = 0;
        for (int i = 1; i < vertices.size(); i++)
            length += vectorModule(vertices[i - 1], vertices[i]);
        return length;
    }

    virtual void print () const
    {
        std::cout << "This polyline contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (Point p: vertices)
            std::cout << "(" << p.getX() << "; " << p.getY() << ") ";
        std::cout << ".\nLength of your polyline is: " << getLength() << ".\n\n";
    }
};

class ClosedPolyline: public Polyline
{
public:
    ClosedPolyline() = default;
    explicit ClosedPolyline(std::vector<Point> &arr): Polyline(arr)
    { assert(getClosed() && "Polyline isn't closed"); }
    explicit ClosedPolyline(Point arr[], int size): Polyline(arr, size) { assert(getClosed() && "Polyline isn't closed"); }


    bool getClosed() const
    {
        if (vertices.front().getY() == vertices.back().getY() && vertices.front().getX() == vertices.back().getY())
            return true;
        else return false;
    }

    void print () const override
    {
        std::cout << "This closed polyline contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nLength of your closed polyline is: " << getLength() << ".\n\n";
    }
};

class Polygon: public ClosedPolyline
{
protected:
    struct Edge
    {
        double module;
        Point cords;
    };
    std::vector<Edge> edges;
    std::vector<double> angles;
public:
    Polygon(): ClosedPolyline() {}

    explicit Polygon(std::vector<Point> &arr): ClosedPolyline(arr)
    { assert(getClosed() && "Part of the plane is not limited by a closed polyline"); }

    explicit Polygon(Point arr[], int size): ClosedPolyline(arr, size)
    { assert(getClosed() && "Part of the plane is not limited by a closed polyline"); }

    double scalarComposition(const Point &a, const Point &b)
    {
        return a.getX()*b.getX() + a.getY()*b.getY();
    }

    Point vectorCords(const Point &a, const Point &b)
    {
        return Point {b.getX() - a.getX(), b.getY() - a.getY()};
    }

    void getEdges()
    {
        for (int i = 1; i < vertices.size(); i++)
            edges.push_back({vectorModule(vertices[i - 1], vertices[i]),
                             vectorCords(vertices[i - 1], vertices[i])});
    }

    void getAngles()
    {
        for (int i = 1; i < edges.size(); i++)
        {
            double sc = scalarComposition(edges[i - 1].cords, edges[i].cords);

            double mod1 = edges[i - 1].module;
            double mod2 = edges[i].module;

            angles.emplace_back(acos(sc / mod1*mod2));
        }
    }

    virtual double getPerimeter() const
    {
        return ClosedPolyline::getLength();
    }

    virtual double getArea() const
    {
        double sum = 0;

        for (int i = 1; i < vertices.size(); i++)
        {
            sum += (vertices[i].getX() + vertices[i - 1].getX())*(vertices[i - 1].getY() - vertices[i].getY());
        }

        return abs(sum) / 2;
    }

    void print() const override
    {
        std::cout << "This polygon contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nPerimeter of your polygon is: " << getPerimeter() << ".";
        std::cout << "\nArea of your polygon is: " << getArea() << ".\n\n";
    }
};

class RegularPolygon: public Polygon
{
public:
    RegularPolygon(): Polygon() {}

    explicit RegularPolygon(std::vector<Point> &arr): Polygon(arr)
    { assert(checkRegular() && "Your polygon isn't regular!"); }

    explicit RegularPolygon(Point arr[], int size): Polygon(arr, size)
    { assert(checkRegular() && "Your polygon isn't regular!"); }

    bool checkRegular()
    {
        getEdges();
        getAngles();

        bool flag = true;
        for (int i = 1; i < edges.size(); i++)
            if (edges[i - 1].module != edges[i].module)
            {
                flag = false;
                return flag;
            }
        for (int i = 1; i < angles.size(); i++)
            if (angles[i - 1] != angles[i])
            {
                flag = false;
                return flag;
            }
        return flag;
    }

    void print() const override
    {
        std::cout << "This regular polygon contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nPerimeter of your regular polygon is: " << getPerimeter() << ".";
        std::cout << "\nArea of your regular polygon is: " << getArea() << ".\n\n";
    }
};

class Triangle: public Polygon
{
public:
    Triangle(): Polygon() {}
    explicit Triangle(std::vector<Point> &arr): Polygon(arr)
    { assert(checkTriangle() && "Your polygon isn't a triangle!"); }

    bool checkTriangle()
    {
        getEdges();
        if (edges.size() != 3)
            return false;

        double a = edges[0].module;
        double b = edges[1].module;
        double c = edges[2].module;

        if (a + b > c && a + c > b && b + c > a)
            return true;
        else return false;
    }

    void print() const override
    {
        std::cout << "This triangle contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nPerimeter of your triangle is: " << getPerimeter() << ".";
        std::cout << "\nArea of your triangle is: " << getArea() << ".\n\n";
    }

};

class Trapezoid: public Polygon
{
public:
    Trapezoid():Polygon() {}
    explicit Trapezoid(std::vector<Point> &arr): Polygon(arr)
    { assert(trapezoidCheck() && "Your polygon isn't a trapezoid!"); }

    bool trapezoidCheck()
    {
        getEdges();
        double anglecoeff1 = abs((vertices[1].getY() - vertices[0].getY()) / (vertices[1].getX() - vertices[0].getX() + INT16_MAX));
        double anglecoeff3 = abs((vertices[3].getY() - vertices[2].getY()) / (vertices[3].getX() - vertices[2].getX() + INT16_MAX));

        double anglecoeff2 = abs((vertices[2].getY() - vertices[1].getY()) / (vertices[2].getX() - vertices[1].getX() + INT16_MAX));
        double anglecoeff4 = abs((vertices[3].getY() - vertices[0].getY()) / (vertices[3].getX() - vertices[0].getX() + INT16_MAX));

        if (((anglecoeff1 == anglecoeff3) || (anglecoeff2 == anglecoeff4)) && (edges.size() == 4)) return true;

        return false;
    }

    void print() const override
    {
        std::cout << "This trapezoid contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nPerimeter of your trapezoid is: " << getPerimeter() << ".";
        std::cout << "\nArea of your trapezoid is: " << getArea() << ".\n\n";
    }
};