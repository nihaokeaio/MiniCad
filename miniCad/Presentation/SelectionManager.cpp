//
// Created by ZQD on 2026/5/31.
//

#include "SelectionManager.h"

void SelectionManager::SetSelected(const ElementId id) {
    m_Selected.insert(id);
}

void SelectionManager::Clear() {
    m_Selected.clear();
}

bool SelectionManager::IsSelected(const ElementId id) const {
    return m_Selected.contains(id);
}

const std::unordered_set<ElementId> &SelectionManager::Selected() const {
    return m_Selected;
}
