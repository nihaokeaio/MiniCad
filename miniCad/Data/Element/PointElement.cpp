//
// Created by ZQD on 2026/6/11.
//

#include "PointElement.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>

PointElement::PointElement() {
    m_Name = "PointElement";
}

TopoDS_Shape PointElement::BuildShape() const {
    return BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 0.0, 0.0));
}
