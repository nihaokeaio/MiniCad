//
// Created by ZQD on 2026/6/8.
//

#pragma once

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <optional>


namespace GeomCalculator {
    ///计算射线与平面的交点
    Standard_EXPORT std::optional<gp_Pnt> CalculatorRayIntsPlane(const gp_Ax1 &ray, const gp_Pln &plane);
};


