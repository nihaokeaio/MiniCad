//
// Created by ZQD on 2026/5/31.
//

#pragma once

#include <unordered_set>

#include "../Data/Element/ElementId.h"
#include "Controller/Interaction/Picking/PickTypes.h"


class SelectionManager {
public:
    void SetSelected(ElementId id);

    void SetSelected(const ElementPickTarget &target);

    void Clear();

    bool IsSelected(ElementId id) const;

    bool IsSelected(const ElementPickTarget &target) const;

    bool HasSelection() const;

    ElementId GetSingleSelected() const;

    ElementPickTarget GetSingleSelectedTarget() const;

    const std::unordered_set<ElementId> &Selected() const;

    const std::unordered_set<ElementPickTarget> &SelectedTargets() const;

private:
    std::unordered_set<ElementId> m_Selected;
    std::unordered_set<ElementPickTarget> m_SelectedTargets;
};


