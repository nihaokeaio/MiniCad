//
// Created by ZQD on 2026/6/7.
//

#include "PreviewManager.h"

void PreviewManager::BeginElementPreview(const ElementCreateParams &params) {
    m_CurrentParams = params;
    m_HasPreview = true;
}

void PreviewManager::UpdateElementPreviewPosition(const std::vector<double> &position) {
    if (!m_HasPreview) {
        return;
    }
    m_CurrentParams.properties["Position"] = PropertyValue(position);
}

void PreviewManager::Clear() {
    m_HasPreview = false;
}

bool PreviewManager::HasPreview() const {
    return m_HasPreview;
}
