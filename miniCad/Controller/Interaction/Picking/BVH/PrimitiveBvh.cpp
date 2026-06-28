//
// Created by ZQD on 2026/6/13.
//

#include "PrimitiveBvh.h"

#include "BvhTree.h"

#include <algorithm>
#include <QDebug>

#include "Timer.h"

class PrimitiveBvh::PImpl {
public:
    PImpl()
        : m_Tree(
            [](const PickPrimitive &, uint32_t itemIndex) -> std::optional<uint32_t> {
                return itemIndex;
            },
            [](const PickSettings &settings) {
                return std::max(settings.pointTolerance, settings.segmentTolerance);
            }) {
    }

    void Build(const std::vector<PickPrimitive> &primitives) {
        m_Tree.Build(primitives);
    }

    [[nodiscard]] std::vector<uint32_t> Query(const PickQuery &query) const {
        return m_Tree.Query(query);
    }

    [[nodiscard]] bool Empty() const {
        return m_Tree.Empty();
    }

private:
    Picking::BvhTree<PickPrimitive, uint32_t> m_Tree;
};

PrimitiveBvh::PrimitiveBvh() : m_Impl(std::make_unique<PImpl>()) {
}

PrimitiveBvh::~PrimitiveBvh() = default;

PrimitiveBvh::PrimitiveBvh(PrimitiveBvh &&) noexcept = default;

PrimitiveBvh &PrimitiveBvh::operator=(PrimitiveBvh &&) noexcept = default;

void PrimitiveBvh::Build(const std::vector<PickPrimitive> &primitives) const {
    m_Impl->Build(primitives);
}

std::vector<uint32_t> PrimitiveBvh::Query(const PickQuery &query) const {
    return m_Impl->Query(query);
}

bool PrimitiveBvh::Empty() const {
    return m_Impl->Empty();
}
