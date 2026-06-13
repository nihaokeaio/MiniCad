//
// Created by ZQD on 2026/6/13.
//

#include "ObjectBvh.h"

#include "BvhTree.h"
#include "Timer.h"
#include "QDebug"

class ObjectBvh::PImpl {
public:
    PImpl()
        : m_Tree(
              [](const PickObject &object, uint32_t) -> std::optional<ElementId> {
                  if (!object.elementId.IsValid()) {
                      return std::nullopt;
                  }
                  return object.elementId;
              },
              [](const PickSettings &settings) {
                  return settings.broadPhaseTolerance;
              }) {
    }

    void Build(const std::vector<PickObject> &objects) {
        m_Tree.Build(objects);
    }

    [[nodiscard]] std::vector<ElementId> Query(const PickQuery &query) const {
        return m_Tree.Query(query);
    }

    [[nodiscard]] bool Empty() const {
        return m_Tree.Empty();
    }

private:
    Picking::BvhTree<PickObject, ElementId> m_Tree;
};

ObjectBvh::ObjectBvh() : m_Impl(std::make_unique<PImpl>()) {
}

ObjectBvh::~ObjectBvh() = default;

ObjectBvh::ObjectBvh(ObjectBvh &&) noexcept = default;

ObjectBvh &ObjectBvh::operator=(ObjectBvh &&) noexcept = default;

void ObjectBvh::Build(const std::vector<PickObject> &objects) const {
    Timer timer;
    m_Impl->Build(objects);
    qDebug() << "[PickBenchmark]" << "Build ObjectBvh :" << timer.ElapsedMicroseconds() << "us";
}

std::vector<ElementId> ObjectBvh::Query(const PickQuery &query) const {
    return m_Impl->Query(query);
}

bool ObjectBvh::Empty() const {
    return m_Impl->Empty();
}
