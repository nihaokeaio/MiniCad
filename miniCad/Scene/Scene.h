//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../Data/Element/ElementId.h"
#include "SceneElement.h"

class Element;

class Scene {
public:
    void AddOrUpdate(const Element &element);

    void AddWidget(const ElementMesh &elementMesh, const GizmoHandleId &handle);

    void SetWidgetTransform(const GeometryTypes::RTransform &worldTransform);

    void Remove(ElementId elementId);

    [[nodiscard]] const SceneElement *FindObject(ElementId elementId) const;

    const std::unordered_map<ElementId, std::unique_ptr<SceneElement> > &GetAllObjects() const;

    const std::vector<std::unique_ptr<SceneWidget> > &GetAllWidgets() const;

    [[nodiscard]] std::vector<ElementId> GetPickCandidates() const;

    [[nodiscard]] bool Contains(ElementId elementId) const;

    [[nodiscard]] std::vector<PickObject> BuildPickObjects() const;

    [[nodiscard]] uint64_t GetVersion() const;

    void Clear();

    void ClearWidgets();

    void ClearObjects();

private:
    std::unordered_map<ElementId, std::unique_ptr<SceneElement> > m_Objects;
    std::vector<std::unique_ptr<SceneWidget> > m_Widgets;
    uint64_t m_Version{0};
};
