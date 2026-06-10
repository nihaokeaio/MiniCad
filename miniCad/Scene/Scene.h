//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../Data/Element/ElementId.h"
#include "SceneObject.h"

class Element;

class Scene {
public:
    void AddOrUpdate(const Element &element);

    void Remove(ElementId elementId);

    [[nodiscard]] const SceneObject *FindObject(ElementId elementId) const;
    const std::unordered_map<ElementId, std::unique_ptr<SceneObject>>& GetAllObjects() const;
    [[nodiscard]] std::vector<ElementId> GetPickCandidates() const;

    [[nodiscard]] bool Contains(ElementId elementId) const;

    void Clear();

private:
    std::unordered_map<ElementId, std::unique_ptr<SceneObject>> m_Objects;
};
