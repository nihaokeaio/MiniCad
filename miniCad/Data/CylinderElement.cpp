//
// Created by ZQD on 2026/6/3.
//

#include "CylinderElement.h"
#include <BRepPrimAPI_MakeCylinder.hxx>

CylinderElement::CylinderElement() {
    m_Name = "CylinderElement";
    m_Properties.SetT("Radius", 100.0);
    m_Properties.SetT("Height", 100.0);
}

TopoDS_Shape CylinderElement::BuildShape() const {
    return ApplyPlacement(BRepPrimAPI_MakeCylinder(GetRadius(), GetHeight()));
}
