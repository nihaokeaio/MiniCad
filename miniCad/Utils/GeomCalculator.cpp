//
// Created by ZQD on 2026/6/8.
//

#include "GeomCalculator.h"
#include <V3d_View.hxx>

std::optional<gp_Pnt> GeomCalculator::CalculatorRayIntsPlane(const gp_Ax1 &ray, const gp_Pln &plane) {
    // 获取射线的原点和方向
    gp_Pnt rayOrigin = ray.Location();
    gp_Dir rayDir = ray.Direction();

    // 获取平面的法向和通过点
    gp_Dir planeNormal = plane.Axis().Direction();
    gp_Pnt planePoint = plane.Location();

    // 计算射线方向与平面法向的点积
    double denominator = rayDir.Dot(planeNormal);

    // 如果点积接近零，射线与平面平行或共面，无交点
    if (std::abs(denominator) < gp::Resolution()) {
        return std::nullopt;
    }

    // 计算从射线原点到平面上任意点的向量
    gp_Vec rayToPlane(rayOrigin, planePoint);

    // 计算参数 t = ( (P0 - O) · N ) / (D · N)
    // 其中 P0 是平面上一点，O 是射线原点，N 是平面法向，D 是射线方向
    double t = rayToPlane.Dot(planeNormal) / denominator;

    // 对于射线（ray），通常要求 t >= 0
    // 如果 t < 0，交点在射线反方向，不应考虑
    if (t < 0.0) {
        return std::nullopt;
    }
    // 计算交点坐标
    gp_Pnt intersectionPoint = rayOrigin.Translated(rayDir.XYZ() * t);

    // 可选：验证交点是否在平面上（数值稳定性检查）
    // double distance = plane.Distance(intersectionPoint);
    // if (distance > 1e-6) return std::nullopt;

    return intersectionPoint;
}

bool GeomCalculator::RayIntersectBox(const Bnd_Box &box, const gp_Lin &ray, double &tmin, double &tmax) {
    tmin = 0.0;
    tmax = Precision::Infinite();

    double aabbMin[3], aabbMax[3];
    box.Get(aabbMin[0], aabbMin[1], aabbMin[2], aabbMax[0], aabbMax[1], aabbMax[2]);

    const gp_Pnt &O = ray.Location();
    const gp_Dir &D = ray.Direction();

    for (int i = 0; i < 3; ++i) {
        if (Abs(D.Coord(i + 1)) < Precision::Angular()) {
            // 射线平行于当前轴的平面
            if (O.Coord(i + 1) < aabbMin[i] || O.Coord(i + 1) > aabbMax[i]) {
                return false; // 不相交
            }
        } else {
            // 计算当前轴的两个交点参数
            double invD = 1.0 / D.Coord(i + 1);
            double t1 = (aabbMin[i] - O.Coord(i + 1)) * invD;
            double t2 = (aabbMax[i] - O.Coord(i + 1)) * invD;

            // 确保t1是近交点，t2是远交点
            if (t1 > t2)
                std::swap(t1, t2);

            // 更新总体tmin和tmax
            tmin = Max(t1, tmin);
            tmax = Min(t2, tmax);

            // 检查是否已无交集
            if (tmin > tmax)
                return false;
        }
    }

    return true;
}

gp_Ax1 GeomCalculator::GetMouseScreenRay(int x, int y, const opencascade::handle<V3d_View> &view) {
    Standard_Real worldX, worldY, worldZ;
    Standard_Real dirX, dirY, dirZ;
    view->ConvertWithProj(x, y, worldX, worldY, worldZ, dirX, dirY, dirZ);
    return {gp_Pnt(worldX, worldY, worldZ), gp_Dir(dirX, dirY, dirZ)};
}
