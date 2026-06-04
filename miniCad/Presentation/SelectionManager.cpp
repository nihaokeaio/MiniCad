//
// Created by ZQD on 2026/5/31.
//

#include "SelectionManager.h"

void SelectionManager::SetSelected(const ElementId id) {
    if (!id.IsValid()) {
        return;
    }
    m_Selected.clear();
    m_Selected.insert(id);
}

void SelectionManager::Clear() {
    m_Selected.clear();
}

bool SelectionManager::IsSelected(const ElementId id) const {
    return m_Selected.contains(id);
}

bool SelectionManager::HasSelection() const {
    return !m_Selected.empty();
}

ElementId SelectionManager::GetSingleSelected() const {
    if (m_Selected.empty()) {
        return ElementId::InvalidId;
    }
    return *m_Selected.begin();
}

const std::unordered_set<ElementId> &SelectionManager::Selected() const {
    return m_Selected;
}
