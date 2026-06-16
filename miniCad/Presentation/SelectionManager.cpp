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
    m_SelectedTargets.clear();
    m_SelectedTargets.insert(PickTarget{id, MeshPrimitiveType::Object, InvalidPrimitiveIndex});
}

void SelectionManager::SetSelected(const PickTarget &target) {
    if (!target.elementId.IsValid()) {
        return;
    }
    m_Selected.clear();
    m_Selected.insert(target.elementId);
    m_SelectedTargets.clear();
    m_SelectedTargets.insert(target);
}

void SelectionManager::Clear() {
    m_Selected.clear();
    m_SelectedTargets.clear();
}

bool SelectionManager::IsSelected(const ElementId id) const {
    return m_Selected.contains(id);
}

bool SelectionManager::IsSelected(const PickTarget &target) const {
    return m_SelectedTargets.contains(target);
}

bool SelectionManager::HasSelection() const {
    return !m_SelectedTargets.empty();
}

ElementId SelectionManager::GetSingleSelected() const {
    if (m_Selected.empty()) {
        return ElementId::InvalidId;
    }
    return *m_Selected.begin();
}

PickTarget SelectionManager::GetSingleSelectedTarget() const {
    if (m_SelectedTargets.empty()) {
        return {};
    }
    return *m_SelectedTargets.begin();
}

const std::unordered_set<ElementId> &SelectionManager::Selected() const {
    return m_Selected;
}

const std::unordered_set<PickTarget> &SelectionManager::SelectedTargets() const {
    return m_SelectedTargets;
}
