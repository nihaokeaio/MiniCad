//
// Created by ZQD on 2026/6/13.
//

#include "PrimitivePicker.h"

#include "GeomCalculator.h"
#include "Scene/SceneObject.h"

#include <algorithm>
#include <numeric>

#include <gp_Trsf.hxx>

std::optional<PickResult> PrimitivePicker::Pick(const PickQuery &localQuery, const SceneObject &object) const {
    std::optional<PickResult> best;

    const auto primitiveIndices = (!localQuery.settings.usePrimitiveBvh || object.primitiveBvh.Empty())
                                      ? LinearPrimitiveIndices(object)
                                      : object.primitiveBvh.Query(localQuery);

    for (const auto primitiveVectorIndex: primitiveIndices) {
        if (primitiveVectorIndex >= object.pickPrimitives.size()) {
            continue;
        }

        const auto &primitive = object.pickPrimitives[primitiveVectorIndex];
        std::optional<PickResult> candidate;
        switch (primitive.primitiveType) {
            case MeshPrimitiveType::Point:
                candidate = PickPoint(localQuery, object, primitive);
                break;
            case MeshPrimitiveType::Segment:
                candidate = PickSegment(localQuery, object, primitive);
                break;
            case MeshPrimitiveType::Triangle:
                candidate = PickTriangle(localQuery, object, primitive);
                break;
            case MeshPrimitiveType::Object:
            case MeshPrimitiveType::None:
            default:
                break;
        }

        if (candidate.has_value()) {
            Picking::UpdateBestPick(best, *candidate);
        }
    }

    return best;
}

std::optional<PickResult> PrimitivePicker::PickPoint(const PickQuery &localQuery,
                                                     const SceneObject &object,
                                                     const PickPrimitive &primitive) const {
    const auto &mesh = object.pickGeometry;
    if (primitive.primitiveIndex >= mesh.points.size()) {
        return std::nullopt;
    }

    const auto vertexIndex = mesh.points[primitive.primitiveIndex].v0;
    if (vertexIndex >= mesh.vertices.size()) {
        return std::nullopt;
    }

    const auto hit = GeomCalculator::RayIntersectPoint(
        localQuery.ray,
        mesh.vertices[vertexIndex].position,
        localQuery.settings.pointTolerance);
    if (!hit.has_value()) {
        return std::nullopt;
    }

    const gp_Pnt worldRayOrigin = localQuery.ray.Location().Transformed(object.worldTransform);
    const gp_Pnt worldRayPoint = hit->point.Transformed(object.worldTransform);

    PickResult result;
    result.elementId = object.elementId;
    result.primitiveType = MeshPrimitiveType::Point;
    result.primitiveIndex = primitive.primitiveIndex;
    result.hitPoint = gp_Pnt(mesh.vertices[vertexIndex].position).Transformed(object.worldTransform);
    result.distance = worldRayOrigin.Distance(worldRayPoint);
    return result;
}

std::optional<PickResult> PrimitivePicker::PickSegment(const PickQuery &localQuery,
                                                       const SceneObject &object,
                                                       const PickPrimitive &primitive) const {
    const auto &mesh = object.pickGeometry;
    if (primitive.primitiveIndex >= mesh.segments.size()) {
        return std::nullopt;
    }

    const auto &segment = mesh.segments[primitive.primitiveIndex];
    if (segment.v0 >= mesh.vertices.size() || segment.v1 >= mesh.vertices.size()) {
        return std::nullopt;
    }

    const auto hit = GeomCalculator::RayIntersectSegment(
        localQuery.ray,
        mesh.vertices[segment.v0].position,
        mesh.vertices[segment.v1].position,
        localQuery.settings.segmentTolerance);
    if (!hit.has_value()) {
        return std::nullopt;
    }

    const gp_Pnt worldRayOrigin = localQuery.ray.Location().Transformed(object.worldTransform);
    const gp_Pnt worldRayPoint = hit->point.Transformed(object.worldTransform);

    PickResult result;
    result.elementId = object.elementId;
    result.primitiveType = MeshPrimitiveType::Segment;
    result.primitiveIndex = primitive.primitiveIndex;
    result.hitPoint = hit->segmentPoint.Transformed(object.worldTransform);
    result.distance = worldRayOrigin.Distance(worldRayPoint);
    return result;
}

std::optional<PickResult> PrimitivePicker::PickTriangle(const PickQuery &localQuery,
                                                        const SceneObject &object,
                                                        const PickPrimitive &primitive) const {
    const auto &mesh = object.pickGeometry;
    if (primitive.primitiveIndex >= mesh.triangles.size()) {
        return std::nullopt;
    }

    const auto &triangle = mesh.triangles[primitive.primitiveIndex];
    if (triangle.v0 >= mesh.vertices.size() ||
        triangle.v1 >= mesh.vertices.size() ||
        triangle.v2 >= mesh.vertices.size()) {
        return std::nullopt;
    }

    const auto hit = GeomCalculator::RayIntersectTriangle(
        localQuery.ray,
        mesh.vertices[triangle.v0].position,
        mesh.vertices[triangle.v1].position,
        mesh.vertices[triangle.v2].position);
    if (!hit.has_value()) {
        return std::nullopt;
    }

    const gp_Pnt worldRayOrigin = localQuery.ray.Location().Transformed(object.worldTransform);

    PickResult result;
    result.elementId = object.elementId;
    result.primitiveType = MeshPrimitiveType::Triangle;
    result.primitiveIndex = primitive.primitiveIndex;
    result.hitPoint = hit->point.Transformed(object.worldTransform);
    result.distance = worldRayOrigin.Distance(result.hitPoint);
    return result;
}

std::vector<uint32_t> PrimitivePicker::LinearPrimitiveIndices(const SceneObject &object) {
    std::vector<uint32_t> indices(object.pickPrimitives.size());
    std::iota(indices.begin(), indices.end(), 0);
    return indices;
}
