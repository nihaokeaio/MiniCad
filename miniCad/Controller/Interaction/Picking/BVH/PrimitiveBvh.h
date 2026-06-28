//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include "Controller/Interaction/Picking/PickTypes.h"

#include <memory>
#include <vector>

class PrimitiveBvh {
public:
    PrimitiveBvh();

    ~PrimitiveBvh();

    PrimitiveBvh(PrimitiveBvh &&) noexcept;

    PrimitiveBvh &operator=(PrimitiveBvh &&) noexcept;

    PrimitiveBvh(const PrimitiveBvh &) = delete;

    PrimitiveBvh &operator=(const PrimitiveBvh &) = delete;

    void Build(const std::vector<PickPrimitive> &primitives) const;

    [[nodiscard]] std::vector<uint32_t> Query(const PickQuery &query) const;

    [[nodiscard]] bool Empty() const;


private:
    class PImpl;
    std::unique_ptr<PImpl> m_Impl;
};
