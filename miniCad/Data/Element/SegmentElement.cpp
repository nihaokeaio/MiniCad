//
// Created by ZQD on 2026/6/11.
//

#include "SegmentElement.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Pnt.hxx>

SegmentElement::SegmentElement() {
    m_Name = "SegmentElement";
    m_Properties.SetT("Length", 100.0);
}

TopoDS_Shape SegmentElement::BuildShape() const {
    const double halfLength = GetLength() * 0.5;
    return BRepBuilderAPI_MakeEdge(
        gp_Pnt(-halfLength, 0.0, 0.0),
        gp_Pnt(halfLength, 0.0, 0.0));
}

double SegmentElement::GetLength() const {
    double value = 0.0;
    GetProperty("Length", value);
    return value;
}
