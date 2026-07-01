//
// Created by ZQD on 2026/7/1.
//

#pragma once

#include <AIS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace PresentationPrimitive {
    struct LineStyle {
        Quantity_Color color;
        double width{1.0};
    };

    Handle(AIS_Shape) MakeLine(const gp_Pnt &from, const gp_Pnt &to, const LineStyle &style);

    Handle(AIS_Shape) MakeCircle(const gp_Pnt &center, const gp_Dir &normal, double radius, const LineStyle &style);

    void ApplyLineStyle(const Handle(AIS_Shape) &shape, const LineStyle &style);
}
