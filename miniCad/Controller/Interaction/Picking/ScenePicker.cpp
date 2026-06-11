//
// Created by ZQD on 2026/6/10.
//

#include "ScenePicker.h"

#include "GeomCalculator.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"

#include <gp_Trsf.hxx>

ScenePicker::ScenePicker(const Scene *scene) : m_Scene(scene) {
}

std::optional<PickResult> ScenePicker::Pick(const gp_Lin &ray) const {
    std::optional<PickResult> best;
    if (m_Scene == nullptr) {
        return best;
    }

    constexpr double pointTolerance = 2.0;
    constexpr double segmentTolerance = 2.0;
    constexpr double broadPhaseTolerance = 2.0;
    for (const auto elementId: m_Scene->GetPickCandidates()) {
        const auto object = m_Scene->FindObject(elementId);
        if (object == nullptr) {
            continue;
        }

        Bnd_Box pickBox = object->boundingBox;
        pickBox.Enlarge(broadPhaseTolerance);

        double tmin = 0.0;
        double tmax = 0.0;
        if (!GeomCalculator::RayIntersectBox(pickBox, ray, tmin, tmax)) {
            continue;
        }

        PickResult candidate = MakeObjectPick(elementId, ray, tmin);
        if (!object->pickGeometry.Empty()) {
            const gp_Trsf worldToLocal = object->worldTransform.Inverted();
            const gp_Lin localRay = ray.Transformed(worldToLocal);
            if (const auto meshHit = PickElementMesh(localRay, *object, pointTolerance, segmentTolerance)) {
                candidate = *meshHit;
            } else if (object->pickGeometry.triangles.empty()) {
                continue;
            }
        }

        UpdateBestPick(best, candidate);
    }

    return best;
}

std::optional<PickResult> ScenePicker::PickElementMesh(const gp_Lin &localRay,
                                                       const SceneObject &object,
                                                       double pointTolerance,
                                                       double segmentTolerance) const {
    std::optional<PickResult> best;

    if (const auto pointHit = PickMeshPoints(localRay, object, pointTolerance)) {
        UpdateBestPick(best, *pointHit);
    }
    if (const auto segmentHit = PickMeshSegments(localRay, object, segmentTolerance)) {
        UpdateBestPick(best, *segmentHit);
    }
    if (const auto triangleHit = PickMeshTriangles(localRay, object)) {
        UpdateBestPick(best, *triangleHit);
    }

    return best;
}

std::optional<PickResult> ScenePicker::PickMeshPoints(const gp_Lin &localRay,
                                                      const SceneObject &object,
                                                      double pointTolerance) const {
    std::optional<PickResult> best;
    const auto &mesh = object.pickGeometry;
    const gp_Pnt worldRayOrigin = localRay.Location().Transformed(object.worldTransform);

    for (uint32_t index = 0; index < mesh.points.size(); ++index) {
        const auto vertexIndex = mesh.points[index].v0;
        if (vertexIndex >= mesh.vertices.size()) {
            continue;
        }

        const auto hit = GeomCalculator::RayIntersectPoint(
            localRay,
            mesh.vertices[vertexIndex].position,
            pointTolerance);
        if (!hit.has_value()) {
            continue;
        }

        PickResult result;
        result.elementId = object.elementId;
        result.primitiveType = MeshPrimitiveType::Point;
        result.primitiveIndex = index;
        const gp_Pnt worldRayPoint = hit->point.Transformed(object.worldTransform);
        result.hitPoint = gp_Pnt(mesh.vertices[vertexIndex].position).Transformed(object.worldTransform);
        result.distance = worldRayOrigin.Distance(worldRayPoint);
        UpdateBestPick(best, result);
    }

    return best;
}

std::optional<PickResult> ScenePicker::PickMeshSegments(const gp_Lin &localRay,
                                                        const SceneObject &object,
                                                        double segmentTolerance) const {
    std::optional<PickResult> best;
    const auto &mesh = object.pickGeometry;
    const gp_Pnt worldRayOrigin = localRay.Location().Transformed(object.worldTransform);

    for (uint32_t index = 0; index < mesh.segments.size(); ++index) {
        const auto &segment = mesh.segments[index];
        if (segment.v0 >= mesh.vertices.size() || segment.v1 >= mesh.vertices.size()) {
            continue;
        }

        const auto hit = GeomCalculator::RayIntersectSegment(
            localRay,
            mesh.vertices[segment.v0].position,
            mesh.vertices[segment.v1].position,
            segmentTolerance);
        if (!hit.has_value()) {
            continue;
        }

        PickResult result;
        result.elementId = object.elementId;
        result.primitiveType = MeshPrimitiveType::Segment;
        result.primitiveIndex = index;
        const gp_Pnt worldRayPoint = hit->point.Transformed(object.worldTransform);
        result.hitPoint = hit->segmentPoint.Transformed(object.worldTransform);
        result.distance = worldRayOrigin.Distance(worldRayPoint);
        UpdateBestPick(best, result);
    }

    return best;
}

std::optional<PickResult> ScenePicker::PickMeshTriangles(const gp_Lin &localRay,
                                                         const SceneObject &object) const {
    std::optional<PickResult> best;
    const auto &mesh = object.pickGeometry;
    const gp_Pnt worldRayOrigin = localRay.Location().Transformed(object.worldTransform);

    for (uint32_t index = 0; index < mesh.triangles.size(); ++index) {
        const auto &triangle = mesh.triangles[index];
        if (triangle.v0 >= mesh.vertices.size() ||
            triangle.v1 >= mesh.vertices.size() ||
            triangle.v2 >= mesh.vertices.size()) {
            continue;
        }

        const auto hit = GeomCalculator::RayIntersectTriangle(
            localRay,
            mesh.vertices[triangle.v0].position,
            mesh.vertices[triangle.v1].position,
            mesh.vertices[triangle.v2].position);
        if (!hit.has_value()) {
            continue;
        }

        PickResult result;
        result.elementId = object.elementId;
        result.primitiveType = MeshPrimitiveType::Triangle;
        result.primitiveIndex = index;
        result.hitPoint = hit->point.Transformed(object.worldTransform);
        result.distance = worldRayOrigin.Distance(result.hitPoint);
        UpdateBestPick(best, result);
    }

    return best;
}

PickResult ScenePicker::MakeObjectPick(ElementId elementId, const gp_Lin &ray, double distance) {
    PickResult result;
    result.elementId = elementId;
    result.primitiveType = MeshPrimitiveType::Object;
    result.distance = distance;
    result.hitPoint = ray.Location().Translated(ray.Direction().XYZ() * distance);
    return result;
}

bool ScenePicker::IsBetterPick(const PickResult &candidate, const PickResult &current) {
    const auto priority = [](MeshPrimitiveType type) {
        switch (type) {
            case MeshPrimitiveType::Point:
                return 3;
            case MeshPrimitiveType::Segment:
                return 2;
            case MeshPrimitiveType::Triangle:
                return 1;
            case MeshPrimitiveType::Object:
                return 0;
            case MeshPrimitiveType::None:
            default:
                return -1;
        }
    };

    const int candidatePriority = priority(candidate.primitiveType);
    const int currentPriority = priority(current.primitiveType);
    if (candidatePriority != currentPriority) {
        return candidatePriority > currentPriority;
    }
    return candidate.distance < current.distance;
}

void ScenePicker::UpdateBestPick(std::optional<PickResult> &best, const PickResult &candidate) {
    if (!best.has_value() || IsBetterPick(candidate, *best)) {
        best = candidate;
    }
}
