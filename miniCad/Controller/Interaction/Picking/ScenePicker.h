//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <memory>
#include <optional>

#include "BVH/ObjectBvh.h"
#include "PickTypes.h"
#include "PrimitivePicker.h"

class Scene;

class ScenePicker {
public:
    explicit ScenePicker(const Scene *scene);

    ~ScenePicker();

    std::optional<PickResult> Pick(const gp_Lin &ray);

    std::optional<PickResult> Pick(const PickQuery &query);


    std::optional<PickResult> PickWidget(const PickQuery &query) const;

private:
    [[nodiscard]] static PickResult MakeObjectPick(ElementId elementId,
                                                   const PickQuery &query,
                                                   double distance);

    const Scene *m_Scene = nullptr;
    PrimitivePicker m_PrimitivePicker;
    ObjectBvh m_ObjectBvh;
    uint64_t m_ObjectBvhVersion{0};
};
