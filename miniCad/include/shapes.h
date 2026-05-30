#ifndef SHAPES_H
#define SHAPES_H

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

class Shape {
public:
    virtual ~Shape() = default;
    virtual TopoDS_Shape toTopoDS() const = 0;
};

class Circle : public Shape {
public:
    Circle(double radius = 10.0);

    void setPosition(double x, double y);
    void setRadius(double radius);

    TopoDS_Shape toTopoDS() const override;

private:
    gp_Pnt m_center;
    double m_radius;
};

class Rectangle : public Shape {
public:
    Rectangle(double width = 20.0, double height = 10.0);

    void setPosition(double x, double y);
    void setSize(double width, double height);

    TopoDS_Shape toTopoDS() const override;

private:
    gp_Pnt m_corner;
    double m_width;
    double m_height;
};

#endif // SHAPES_H