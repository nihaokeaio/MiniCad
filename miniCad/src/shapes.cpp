#include "shapes.h"

#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <cmath>

static constexpr double PI = 3.14159265358979323846;

Circle::Circle(double radius)
    : m_center(0, 0, 0)
    , m_radius(radius)
{
}

void Circle::setPosition(double x, double y) {
    m_center.SetX(x);
    m_center.SetY(y);
}

void Circle::setRadius(double radius) {
    m_radius = radius;
}

TopoDS_Shape Circle::toTopoDS() const {
    gp_Ax2 axis(m_center, gp::DZ());
    Handle(Geom_Circle) geomCircle = new Geom_Circle(axis, m_radius);
    Handle(Geom_TrimmedCurve) trimmed = new Geom_TrimmedCurve(geomCircle, 0.0, 2.0 * PI);
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(trimmed);
    return edge;
}

Rectangle::Rectangle(double width, double height)
    : m_corner(0, 0, 0)
    , m_width(width)
    , m_height(height)
{
}

void Rectangle::setPosition(double x, double y) {
    m_corner.SetX(x);
    m_corner.SetY(y);
}

void Rectangle::setSize(double width, double height) {
    m_width = width;
    m_height = height;
}

TopoDS_Shape Rectangle::toTopoDS() const {
    BRepBuilderAPI_MakePolygon polygon;
    polygon.Add(m_corner);
    polygon.Add(gp_Pnt(m_corner.X() + m_width, m_corner.Y(), 0));
    polygon.Add(gp_Pnt(m_corner.X() + m_width, m_corner.Y() + m_height, 0));
    polygon.Add(gp_Pnt(m_corner.X(), m_corner.Y() + m_height, 0));
    polygon.Close();
    return polygon.Wire();
}