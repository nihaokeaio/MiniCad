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

ScenePicker::~ScenePicker() = default;

std::optional<PickResult> ScenePicker::Pick(const gp_Lin &ray) {
    return Pick(PickQuery{ray, PickSettings{}});
}

std::optional<PickResult> ScenePicker::Pick(const PickQuery &query) {
    std::optional<PickResult> best;
    if (m_Scene == nullptr) {
        return best;
    }

    if (m_ObjectBvhVersion != m_Scene->GetVersion()) {
        m_ObjectBvh.Build(m_Scene->BuildPickObjects());
        m_ObjectBvhVersion = m_Scene->GetVersion();
    }

    const auto candidates = m_ObjectBvh.Empty()
                                ? m_Scene->GetPickCandidates()
                                : m_ObjectBvh.Query(query);
    for (const auto elementId: candidates) {
        const auto object = m_Scene->FindObject(elementId);
        if (object == nullptr) {
            continue;
        }

        Bnd_Box pickBox = object->boundingBox;
        pickBox.Enlarge(query.settings.broadPhaseTolerance);

        double tmin = 0.0;
        double tmax = 0.0;
        if (!GeomCalculator::RayIntersectBox(pickBox, query.ray, tmin, tmax)) {
            continue;
        }

        std::optional<PickResult> candidate;
        if (Picking::Allows(query.settings.pickMask, MeshPrimitiveType::Object)) {
            candidate = MakeObjectPick(elementId, query, tmin);
        }

        if (Picking::AllowsPrimitive(query.settings.pickMask) && !object->pickGeometry.Empty()) {
            const gp_Trsf worldToLocal = object->worldTransform.Inverted();
            PickQuery localQuery{query.ray.Transformed(worldToLocal), query.settings};
            if (const auto primitiveHit = m_PrimitivePicker.Pick(localQuery, *object)) {
                candidate = *primitiveHit;
            } else if (!candidate.has_value() || object->pickGeometry.triangles.empty()) {
                continue;
            }
        }

        if (candidate.has_value()) {
            Picking::UpdateBestPick(best, *candidate);
        }
    }
    ///线性遍历
#ifdef Debug
    for (const auto elementId: m_Scene->GetPickCandidates()) {
        const auto object = m_Scene->FindObject(elementId);
        if (object == nullptr) {
            continue;
        }

        Bnd_Box pickBox = object->boundingBox;
        pickBox.Enlarge(query.settings.broadPhaseTolerance);

        double tmin = 0.0;
        double tmax = 0.0;
        if (!GeomCalculator::RayIntersectBox(pickBox, query.ray, tmin, tmax)) {
            continue;
        }

        std::optional<PickResult> candidate;
        if (Picking::Allows(query.settings.pickMask, MeshPrimitiveType::Object)) {
            candidate = MakeObjectPick(elementId, query, tmin);
        }

        if (Picking::AllowsPrimitive(query.settings.pickMask) && !object->pickGeometry.Empty()) {
            const gp_Trsf worldToLocal = object->worldTransform.Inverted();
            PickQuery localQuery{query.ray.Transformed(worldToLocal), query.settings};
            if (const auto primitiveHit = m_PrimitivePicker.Pick(localQuery, *object)) {
                candidate = *primitiveHit;
            } else if (!candidate.has_value() || object->pickGeometry.triangles.empty()) {
                continue;
            }
        }

        if (candidate.has_value()) {
            Picking::UpdateBestPick(best, *candidate);
        }
    }
#endif

    return best;
}

PickResult ScenePicker::MakeObjectPick(ElementId elementId, const PickQuery &query, double distance) {
    PickResult result;
    result.pickTarget = PickTarget{elementId, MeshPrimitiveType::Object, InvalidPrimitiveIndex};
    result.distance = distance;
    result.hitPoint = query.ray.Location().Translated(query.ray.Direction().XYZ() * distance);
    return result;
}
