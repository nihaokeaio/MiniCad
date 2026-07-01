//
// Created by ZQD on 2026/7/1.
//

#include "PresentationPrimitiveBuilder.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>

namespace PresentationPrimitive {
    Handle(AIS_Shape) MakeLine(const gp_Pnt &from, const gp_Pnt &to, const LineStyle &style) {
        const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(from, to);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ApplyLineStyle(ais, style);
        return ais;
    }

    Handle(AIS_Shape) MakeCircle(const gp_Pnt &center, const gp_Dir &normal, double radius, const LineStyle &style) {
        const gp_Circ circle(gp_Ax2(center, normal), radius);
        const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ApplyLineStyle(ais, style);
        return ais;
    }

    void ApplyLineStyle(const Handle(AIS_Shape) &shape, const LineStyle &style) {
        if (shape.IsNull()) {
            return;
        }

        shape->SetColor(style.color);
        shape->Attributes()->SetLineAspect(new Prs3d_LineAspect(style.color, Aspect_TOL_SOLID, style.width));
        shape->Attributes()->SetWireAspect(new Prs3d_LineAspect(style.color, Aspect_TOL_SOLID, style.width));
    }
}
