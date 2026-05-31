//
// Created by ZQD on 2026/5/31.
//

#pragma once

#include <unordered_set>
#include "ElementId.h"


class SelectionManager {
public:
    void SetSelected(ElementId id);

    void Clear();

    bool IsSelected(ElementId id) const;

    const std::unordered_set<ElementId> &Selected() const;

private:
    std::unordered_set<ElementId> m_Selected;
};


