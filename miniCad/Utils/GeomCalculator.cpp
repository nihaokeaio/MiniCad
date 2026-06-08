//
// Created by ZQD on 2026/6/8.
//

#include "GeomCalculator.h"

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
