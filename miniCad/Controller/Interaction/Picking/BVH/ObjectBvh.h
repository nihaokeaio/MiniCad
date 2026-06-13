//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include "Controller/Interaction/Picking/PickTypes.h"

#include <memory>
#include <vector>

class ObjectBvh {
public:
    ObjectBvh();

    ~ObjectBvh();

    ObjectBvh(ObjectBvh &&) noexcept;

    ObjectBvh &operator=(ObjectBvh &&) noexcept;

    ObjectBvh(const ObjectBvh &) = delete;

    ObjectBvh &operator=(const ObjectBvh &) = delete;

    void Build(const std::vector<PickObject> &objects) const;

    [[nodiscard]] std::vector<ElementId> Query(const PickQuery &query) const;

    [[nodiscard]] bool Empty() const;

private:
    class PImpl;
    std::unique_ptr<PImpl> m_Impl;
};
