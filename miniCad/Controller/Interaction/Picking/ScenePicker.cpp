//
// Created by ZQD on 2026/6/10.
//

#include "ScenePicker.h"

#include "GeomCalculator.h"
#include "Scene/Scene.h"

ScenePicker::ScenePicker(const Scene *scene) : m_Scene(scene) {
}

std::optional<PickResult> ScenePicker::PickByBoundingBox(const gp_Lin &ray) const {
    std::optional<PickResult> best;
    if (m_Scene == nullptr) {
        return best;
    }

    for (const auto elementId: m_Scene->GetPickCandidates()) {
        const auto object = m_Scene->FindObject(elementId);
        if (object == nullptr) {
            continue;
        }

        double tmin = 0.0;
        double tmax = 0.0;
        if (!GeomCalculator::RayIntersectBox(object->boundingBox, ray, tmin, tmax)) {
            continue;
        }

        if (!best.has_value() || tmin < best->distance) {
            best = PickResult{elementId, tmin};
        }
    }
    return best;
}
