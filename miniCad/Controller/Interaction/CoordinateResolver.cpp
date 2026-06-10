//
// Created by ZQD on 2026/6/8.
//

#include "CoordinateResolver.h"

#include <gp.hxx>
#include <V3d_View.hxx>

#include "GeomCalculator.h"

CoordinateResolver::CoordinateResolver(const opencascade::handle<V3d_View> &view)
    : m_View(view), m_WorkPlan(gp::XOY()) {
}

std::optional<GeometryTypes::Point3D> CoordinateResolver::ScreenToWorkPlane(int x, int y) const {
    const auto ray = GeomCalculator::GetMouseScreenRay(x, y, m_View);
    if (const auto result = GeomCalculator::CalculatorRayIntsPlane(ray, m_WorkPlan); result.has_value()) {
        return result.value().XYZ();
    }
    return std::nullopt;
}
