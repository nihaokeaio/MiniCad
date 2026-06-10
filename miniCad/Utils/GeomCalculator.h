//
// Created by ZQD on 2026/6/8.
//

#pragma once

#include <Bnd_Box.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <optional>


class V3d_View;

namespace GeomCalculator {
    ///计算射线与平面的交点
    Standard_EXPORT std::optional<gp_Pnt> CalculatorRayIntsPlane(const gp_Ax1 &ray, const gp_Pln &plane);

    Standard_EXPORT bool RayIntersectBox(const Bnd_Box &box, const gp_Lin &ray, double &tmin, double &tmax);

    Standard_EXPORT gp_Ax1 GetMouseScreenRay(int x, int y, const Handle(V3d_View) &view);
};


