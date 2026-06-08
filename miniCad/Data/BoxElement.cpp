//
// Created by ZQD on 2026/5/30.
//

#include "BoxElement.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>

BoxElement::BoxElement() {
    m_Name = "BoxElement";
    m_Properties.SetT("Width", 100.0);
    m_Properties.SetT("Height", 100.0);
    m_Properties.SetT("Length", 100.0);
}

TopoDS_Shape BoxElement::BuildShape() const {
    const double halfLength = GetLength() * 0.5;
    const double halfWidth = GetWidth() * 0.5;
    return BRepPrimAPI_MakeBox(
        gp_Pnt(-halfLength, -halfWidth, 0.0),
        gp_Pnt(halfLength, halfWidth, GetHeight()));
}

double BoxElement::GetWidth() const {
    double value = 0.0;
    GetProperty("Width", value);
    return value;
}

double BoxElement::GetHeight() const {
    double value = 0.0;
    GetProperty("Height", value);
    return value;
}

double BoxElement::GetLength() const {
    double value = 0.0;
    GetProperty("Length", value);
    return value;
}
