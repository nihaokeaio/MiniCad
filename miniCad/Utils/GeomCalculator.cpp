//
// Created by ZQD on 2026/6/8.
//

#include "GeomCalculator.h"

#include <algorithm>
#include <cmath>

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

std::optional<GeomCalculator::RayTriangleHit> GeomCalculator::RayIntersectTriangle(const gp_Lin &ray,
                                                                                   const gp_XYZ &v0,
                                                                                   const gp_XYZ &v1,
                                                                                   const gp_XYZ &v2) {
    constexpr double epsilon = 1.0e-9;

    const gp_XYZ origin = ray.Location().XYZ();
    const gp_XYZ direction = ray.Direction().XYZ();
    const gp_XYZ edge1 = v1 - v0;
    const gp_XYZ edge2 = v2 - v0;
    const gp_XYZ pvec = direction.Crossed(edge2);
    const double det = edge1.Dot(pvec);
    if (std::abs(det) < epsilon) {
        return std::nullopt;
    }

    const double invDet = 1.0 / det;
    const gp_XYZ tvec = origin - v0;
    const double u = tvec.Dot(pvec) * invDet;
    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    const gp_XYZ qvec = tvec.Crossed(edge1);
    const double v = direction.Dot(qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    const double distance = edge2.Dot(qvec) * invDet;
    if (distance < 0.0) {
        return std::nullopt;
    }

    gp_XYZ normal = edge1.Crossed(edge2);
    if (normal.SquareModulus() > epsilon) {
        normal.Normalize();
    }

    RayTriangleHit hit;
    hit.distance = distance;
    hit.u = u;
    hit.v = v;
    hit.point = ray.Location().Translated(ray.Direction().XYZ() * distance);
    hit.normal = normal;
    return hit;
}

std::optional<GeomCalculator::RayPointHit> GeomCalculator::RayIntersectPoint(const gp_Lin &ray,
                                                                             const gp_XYZ &point,
                                                                             double tolerance) {
    if (tolerance <= 0.0) {
        return std::nullopt;
    }

    const gp_XYZ origin = ray.Location().XYZ();
    const gp_XYZ direction = ray.Direction().XYZ();
    const gp_XYZ center = point;
    const gp_XYZ offset = origin - center;

    const double a = direction.Dot(direction);
    const double b = 2.0 * direction.Dot(offset);
    const double c = offset.Dot(offset) - tolerance * tolerance;
    const double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) {
        return std::nullopt;
    }

    const double sqrtDiscriminant = std::sqrt(discriminant);
    double t0 = (-b - sqrtDiscriminant) / (2.0 * a);
    double t1 = (-b + sqrtDiscriminant) / (2.0 * a);
    if (t0 > t1) {
        std::swap(t0, t1);
    }

    double distance = t0;
    if (distance < 0.0) {
        distance = t1;
    }
    if (distance < 0.0) {
        return std::nullopt;
    }

    RayPointHit hit;
    hit.distance = distance;
    hit.point = ray.Location().Translated(ray.Direction().XYZ() * distance);
    return hit;
}

std::optional<GeomCalculator::RaySegmentHit> GeomCalculator::RayIntersectSegment(const gp_Lin &ray,
                                                                                 const gp_XYZ &start,
                                                                                 const gp_XYZ &end,
                                                                                 double tolerance) {
    if (tolerance <= 0.0) {
        return std::nullopt;
    }

    const gp_XYZ origin = ray.Location().XYZ();
    const gp_XYZ direction = ray.Direction().XYZ();
    const gp_XYZ segment = end - start;
    const double segmentLengthSquared = segment.SquareModulus();
    if (segmentLengthSquared <= gp::Resolution()) {
        const auto pointHit = RayIntersectPoint(ray, start, tolerance);
        if (!pointHit.has_value()) {
            return std::nullopt;
        }

        RaySegmentHit hit;
        hit.distance = pointHit->distance;
        hit.segmentParameter = 0.0;
        hit.closestDistance = ray.Location().Distance(gp_Pnt(start));
        hit.point = pointHit->point;
        hit.segmentPoint = gp_Pnt(start);
        return hit;
    }

    const gp_XYZ w = origin - start;
    const double a = direction.Dot(direction);
    const double b = direction.Dot(segment);
    const double c = segment.Dot(segment);
    const double d = direction.Dot(w);
    const double e = segment.Dot(w);
    const double denominator = a * c - b * b;

    double segmentParameter = 0.0;
    if (std::abs(denominator) > 1.0e-12) {
        segmentParameter = std::clamp((a * e - b * d) / denominator, 0.0, 1.0);
    } else {
        segmentParameter = std::clamp(-e / c, 0.0, 1.0);
    }

    gp_XYZ segmentPoint = start + segment * segmentParameter;
    double rayDistance = direction.Dot(segmentPoint - origin) / a;
    if (rayDistance < 0.0) {
        rayDistance = 0.0;
        segmentParameter = std::clamp(-e / c, 0.0, 1.0);
        segmentPoint = start + segment * segmentParameter;
    }

    const gp_XYZ rayPoint = origin + direction * rayDistance;
    const double closestDistance = gp_Pnt(rayPoint).Distance(gp_Pnt(segmentPoint));
    if (closestDistance > tolerance) {
        return std::nullopt;
    }

    RaySegmentHit hit;
    hit.distance = rayDistance;
    hit.segmentParameter = segmentParameter;
    hit.closestDistance = closestDistance;
    hit.point = gp_Pnt(rayPoint);
    hit.segmentPoint = gp_Pnt(segmentPoint);
    return hit;
}

gp_Ax1 GeomCalculator::GetMouseScreenRay(int x, int y, const opencascade::handle<V3d_View> &view) {
    Standard_Real worldX, worldY, worldZ;
    Standard_Real dirX, dirY, dirZ;
    view->ConvertWithProj(x, y, worldX, worldY, worldZ, dirX, dirY, dirZ);
    return {gp_Pnt(worldX, worldY, worldZ), gp_Dir(dirX, dirY, dirZ)};
}
