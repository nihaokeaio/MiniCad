//
// Created by ZQD on 2026/6/10.
//

#include "Scene.h"

#include "../Data/Element/Element.h"

void Scene::AddOrUpdate(const Element &element) {
    auto object = std::make_unique<SceneObject>();
    object->elementId = element.GetId();
    object->localTransform = element.GetLocalTransform();
    object->worldTransform = object->localTransform;
    object->boundingBox = element.GetBoundingBox();
    object->pickGeometry = element.BuildElementMesh();
    m_Objects[object->elementId] = std::move(object);
}

void Scene::Remove(ElementId elementId) {
    m_Objects.erase(elementId);
}

const SceneObject *Scene::FindObject(ElementId elementId) const {
    if (const auto iter = m_Objects.find(elementId); iter != m_Objects.end()) {
        return iter->second.get();
    }
    return nullptr;
}

const std::unordered_map<ElementId, std::unique_ptr<SceneObject>> &Scene::GetAllObjects() const {
    return m_Objects;
}

std::vector<ElementId> Scene::GetPickCandidates() const {
    std::vector<ElementId> candidates;
    candidates.reserve(m_Objects.size());
    for (const auto &[elementId, object]: m_Objects) {
        if (object != nullptr && elementId.IsValid()) {
            candidates.push_back(elementId);
        }
    }
    return candidates;
}

bool Scene::Contains(ElementId elementId) const {
    return m_Objects.contains(elementId);
}

void Scene::Clear() {
    m_Objects.clear();
}
