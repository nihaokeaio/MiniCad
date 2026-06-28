//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include <optional>
#include <vector>

#include "PickTypes.h"

struct SceneObject;
struct SceneWidget;
struct SceneElement;

class PrimitivePicker {
public:
    std::optional<PickResult> Pick(const PickQuery &localQuery, const SceneObject &object) const;

private:
    std::optional<PickResult> PickPoint(const PickQuery &localQuery,
                                        const SceneObject &object,
                                        const PickPrimitive &primitive) const;

    std::optional<PickResult> PickSegment(const PickQuery &localQuery,
                                          const SceneObject &object,
                                          const PickPrimitive &primitive) const;

    std::optional<PickResult> PickTriangle(const PickQuery &localQuery,
                                           const SceneObject &object,
                                           const PickPrimitive &primitive) const;

    [[nodiscard]] static std::vector<uint32_t> LinearPrimitiveIndices(const SceneObject &object);

    std::variant<ElementPickTarget, GizmoPickTarget> GetPickTarget(const SceneObject &object, MeshPrimitiveType type,
                                                                   const PickPrimitive &primitive) const;
};
