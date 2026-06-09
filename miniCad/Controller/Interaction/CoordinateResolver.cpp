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

gp_Ax1 CoordinateResolver::GetScreenRay(int x, int y) const {
    Standard_Real worldX, worldY, worldZ;
    Standard_Real dirX, dirY, dirZ;
    m_View->ConvertWithProj(x, y, worldX, worldY, worldZ, dirX, dirY, dirZ);
    return {gp_Pnt(worldX, worldY, worldZ), gp_Dir(dirX, dirY, dirZ)};
}

std::optional<GeometryTypes::Point3D> CoordinateResolver::ScreenToWorkPlane(int x, int y) const {
    auto ray = GetScreenRay(x, y);
    if (auto result = GeomCalculator::CalculatorRayIntsPlane(ray, m_WorkPlan); result.has_value()) {
        return result.value().XYZ();
    }
    return std::nullopt;
}
