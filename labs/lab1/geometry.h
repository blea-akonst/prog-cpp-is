#pragma once

#include <iostream>
#include <cmath>
#include <utility>
#include <vector>
#include <cassert>

class Point {
private:
    double x;
    double y;
public:
    explicit Point(double _x = 0, double _y = 0):
            x(_x), y(_y) {};

    Point (const Point &point) = default;

    Point& operator= (const Point &point) = default;

    double getX() const { return x; }
    double getY() const { return y; }

    void setXY(double _x, double _y) { x = _x; y = _y; }

    void print() const
    {
        std::cout << "Point's cords: " << "(" << getX() << "; " << getY() << ").\n\n";
    }
};

struct Edge
{
    double module;
    Point cords;
};

class Polyline
{
protected:
    std::vector<Point> vertices;
    std::string type;
public:
    explicit Polyline(const std::vector<Point> &points, std::string type = "polyline"): type(std::move(type))
    {
        for (Point p: points)
            vertices.push_back(p);
    }

    Polyline(const Point arr[], int size, std::string type = "polyline"): type(std::move(type))
    {
        for (int i = 0 ; i < size; i++)
            vertices.push_back(arr[i]);
    }

    Polyline& operator= (const Polyline& p)
    {
        this->vertices = p.vertices;

        return *this;
    }

    double vectorModule(const Point &a, const Point &b) const
    {
        double x = b.getX() - a.getX();
        double y = b.getY() - a.getY();
        return sqrt(x*x + y*y);
    }

    double getLength() const
    {
        double length = 0;
        for (int i = 1; i < vertices.size(); i++)
            length += vectorModule(vertices[i - 1], vertices[i]);
        return length;
    }

    void printLine ()
    {
        std::cout << "This " << type << " contains " << vertices.size() - 1 << " vertices with these cords: ";
        for (int i = 0; i < vertices.size() - 1; ++i)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";
        std::cout << ".\nLength of your " << type << " is: " << getLength() << ".\n\n";
    }
};

class ClosedPolyline: public Polyline
{
public:
    explicit ClosedPolyline(std::vector<Point> &arr, std::string type = "closed polyline"): Polyline(arr, std::move(type))
    { assert(getClosed() && "Polyline isn't closed"); }
    explicit ClosedPolyline(Point arr[], int size, std::string type = "closed polyline"): Polyline(arr, size, std::move(type))
    { assert(getClosed() && "Polyline isn't closed"); }

    bool getClosed() const
    {
        if (vertices.front().getY() == vertices.back().getY() && vertices.front().getX() == vertices.back().getY())
            return true;
        else return false;
    }
};

class Polygon: protected ClosedPolyline
{
protected:
    std::vector<Edge> edges;
    std::vector<double> angles;
public:
    explicit Polygon(std::vector<Point> &arr, std::string type = "polygon"): ClosedPolyline(arr, std::move(type))
    { assert(getClosed() && "Part of the plane is not limited by a closed polyline");
      assert(isConvex() && "Your polygon isn't convex!"); }

    explicit Polygon(Point arr[], int size, std::string type = "polygon"): ClosedPolyline(arr, size, std::move(type))
    { assert(getClosed() && "Part of the plane is not limited by a closed polyline");
      assert(isConvex() && "Your polygon isn't convex!"); }

    Polygon& operator= (const Polygon& p)
    {
        this->vertices = p.vertices;
        this->angles = p.angles;
        this->edges = p.edges;

        return *this;
    }

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

    double getPerimeter() const
    {
        return ClosedPolyline::getLength();
    }

    double getArea() const
    {
        double sum = 0;

        for (int i = 1; i < vertices.size(); i++)
        {
            sum += (vertices[i].getX() + vertices[i - 1].getX())*(vertices[i - 1].getY() - vertices[i].getY());
        }

        return abs(sum) / 2;
    }

    bool isConvex()
    {
        for (int i = 1; i < angles.size(); ++i)
        {
            if ( !( (sin(angles[i]) >= 0 && sin(angles[i - 1]) >= 0)
                || (sin(angles[i]) < 0 && sin(angles[i - 1]) < 0) ) ) return false;
            else continue;
        }

        return true;
    }

    void printPolygon()
    {
        std::cout << "This " << type << " contains " << vertices.size() - 1 << " vertices with these cords: ";

        for (int i = 0; i < vertices.size() - 1; i++)
            std::cout << "(" << vertices[i].getX() << "; " << vertices[i].getY() << ") ";

        std::cout << ".\nPerimeter of your " << type << " is: " << getPerimeter() << ".";
        std::cout << "\nArea of your " << type << " is: " << getArea() << ".\n\n";
    }
};

class RegularPolygon: public Polygon
{
public:
    explicit RegularPolygon(std::vector<Point> &arr, std::string type = "regular polygon"): Polygon(arr, std::move(type))
    { assert(checkRegular() && "Your polygon isn't regular!"); }

    explicit RegularPolygon(Point arr[], int size, std::string type = "regular polygon"): Polygon(arr, size, std::move(type))
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
};

class Triangle: public Polygon
{
public:
    explicit Triangle(std::vector<Point> &arr, std::string type = "triangle"): Polygon(arr, std::move(type))
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
};

class Trapezoid: public Polygon
{
public:
    explicit Trapezoid(std::vector<Point> &arr, std::string type = "trapezoid"): Polygon(arr, std::move(type))
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
};

