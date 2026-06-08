//
// Created by ZQD on 2026/6/8.
//

#pragma once
#include <gp_Ax1.hxx>
#include <gp_Pln.hxx>
#include <optional>
#include <Standard_Handle.hxx>

#include "GeometryTypes.h"


class V3d_View;

class CoordinateResolver {
public:
    explicit CoordinateResolver(const Handle(V3d_View) &view);

    gp_Ax1 GetScreenRay(int x, int y) const;

    std::optional<GeometryTypes::Point3D> ScreenToWorkPlane(int x, int y) const;

private:
    Handle(V3d_View) m_View;
    gp_Pln m_WorkPlan;
};


