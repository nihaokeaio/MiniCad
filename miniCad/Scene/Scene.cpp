//
// Created by ZQD on 2026/6/10.
//

#include "Scene.h"

#include "../Data/Element/Element.h"

namespace {
    void AddPointBound(std::vector<PickPrimitive> &primitives, const ElementMesh &mesh, uint32_t index) {
        const auto vertexIndex = mesh.points[index].v0;
        if (vertexIndex >= mesh.vertices.size()) {
            return;
        }

        Bnd_Box bounds;
        bounds.Add(gp_Pnt(mesh.vertices[vertexIndex].position));
        primitives.push_back(PickPrimitive{MeshPrimitiveType::Point, index, bounds});
    }

    void AddSegmentBound(std::vector<PickPrimitive> &primitives, const ElementMesh &mesh, uint32_t index) {
        const auto &segment = mesh.segments[index];
        if (segment.v0 >= mesh.vertices.size() || segment.v1 >= mesh.vertices.size()) {
            return;
        }

        Bnd_Box bounds;
        bounds.Add(gp_Pnt(mesh.vertices[segment.v0].position));
        bounds.Add(gp_Pnt(mesh.vertices[segment.v1].position));
        primitives.push_back(PickPrimitive{MeshPrimitiveType::Segment, index, bounds});
    }

    void AddTriangleBound(std::vector<PickPrimitive> &primitives, const ElementMesh &mesh, uint32_t index) {
        const auto &triangle = mesh.triangles[index];
        if (triangle.v0 >= mesh.vertices.size() ||
            triangle.v1 >= mesh.vertices.size() ||
            triangle.v2 >= mesh.vertices.size()) {
            return;
        }

        Bnd_Box bounds;
        bounds.Add(gp_Pnt(mesh.vertices[triangle.v0].position));
        bounds.Add(gp_Pnt(mesh.vertices[triangle.v1].position));
        bounds.Add(gp_Pnt(mesh.vertices[triangle.v2].position));
        primitives.push_back(PickPrimitive{MeshPrimitiveType::Triangle, index, bounds});
    }

    std::vector<PickPrimitive> BuildPickPrimitives(const ElementMesh &mesh) {
        std::vector<PickPrimitive> primitives;
        primitives.reserve(mesh.points.size() + mesh.segments.size() + mesh.triangles.size());

        for (uint32_t index = 0; index < mesh.points.size(); ++index) {
            AddPointBound(primitives, mesh, index);
        }
        for (uint32_t index = 0; index < mesh.segments.size(); ++index) {
            AddSegmentBound(primitives, mesh, index);
        }
        for (uint32_t index = 0; index < mesh.triangles.size(); ++index) {
            AddTriangleBound(primitives, mesh, index);
        }

        return primitives;
    }
}

void Scene::AddOrUpdate(const Element &element) {
    auto object = std::make_unique<SceneObject>();
    object->elementId = element.GetId();
    object->localTransform = element.GetLocalTransform();
    object->worldTransform = object->localTransform;
    object->boundingBox = element.GetBoundingBox();
    object->pickGeometry = element.BuildElementMesh();
    object->pickPrimitives = BuildPickPrimitives(object->pickGeometry);
    object->primitiveBvh.Build(object->pickPrimitives);
    m_Objects[object->elementId] = std::move(object);
    ++m_Version;
}

void Scene::Remove(ElementId elementId) {
    if (m_Objects.erase(elementId) > 0) {
        ++m_Version;
    }
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

std::vector<PickObject> Scene::BuildPickObjects() const {
    std::vector<PickObject> pickObjects;
    pickObjects.reserve(m_Objects.size());
    for (const auto &[elementId, object]: m_Objects) {
        if (object == nullptr || !elementId.IsValid()) {
            continue;
        }

        pickObjects.push_back(PickObject{elementId, object->boundingBox});
    }
    return pickObjects;
}

uint64_t Scene::GetVersion() const {
    return m_Version;
}

void Scene::Clear() {
    if (!m_Objects.empty()) {
        m_Objects.clear();
        ++m_Version;
    }
}
