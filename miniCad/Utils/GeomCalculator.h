//
// Created by ZQD on 2026/6/8.
//

#pragma once

#include <Bnd_Box.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <optional>


class V3d_View;

namespace GeomCalculator {
    struct RayTriangleHit {
        double distance{Precision::Infinite()};
        double u{0.0};
        double v{0.0};
        gp_Pnt point;
        gp_XYZ normal;
    };

    struct RayPointHit {
        double distance{Precision::Infinite()};
        gp_Pnt point;
    };

    struct RaySegmentHit {
        double distance{Precision::Infinite()};
        double segmentParameter{0.0};
        double closestDistance{Precision::Infinite()};
        gp_Pnt point;
        gp_Pnt segmentPoint;
    };

    ///计算射线与平面的交点
    Standard_EXPORT std::optional<gp_Pnt> CalculatorRayIntsPlane(const gp_Ax1 &ray, const gp_Pln &plane);

    Standard_EXPORT bool RayIntersectBox(const Bnd_Box &box, const gp_Lin &ray, double &tmin, double &tmax);

    Standard_EXPORT std::optional<RayTriangleHit> RayIntersectTriangle(const gp_Lin &ray,
                                                                       const gp_XYZ &v0,
                                                                       const gp_XYZ &v1,
                                                                       const gp_XYZ &v2);

    Standard_EXPORT std::optional<RayPointHit> RayIntersectPoint(const gp_Lin &ray,
                                                                 const gp_XYZ &point,
                                                                 double tolerance);

    Standard_EXPORT std::optional<RaySegmentHit> RayIntersectSegment(const gp_Lin &ray,
                                                                     const gp_XYZ &start,
                                                                     const gp_XYZ &end,
                                                                     double tolerance);

    Standard_EXPORT gp_Ax1 GetMouseScreenRay(int x, int y, const Handle(V3d_View) &view);

    /// @param view 相机
    /// @param targetScreenPixelSize 目标屏幕像素长度
    /// @param pivot 目标在场景中的锚点
    /// @return targetScreenPixelSize 在 pivot 深度处对应的世界空间长度
    Standard_EXPORT double GetPersistenceScreenSize(const Handle(V3d_View) &view, int targetScreenPixelSize,
                                                    const gp_XYZ &pivot);
};

