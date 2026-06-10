//
// Created by ZQD on 2026/6/7.
//

#include "PreviewManager.h"

#include "../../Data/Element/Element.h"
#include "../../Data/Element/ElementFactory.h"
#include "../../Data/Geometry/GeometryTypes.h"
#include "Presentation/PreviewAdaptor.h"

PreviewManager::PreviewManager(PreviewAdaptor *adaptor) : m_Adaptor(adaptor) {
}

void PreviewManager::BeginElementPreview(const ElementCreateParams &params) {
    m_Params = params;
    m_IsPreviewState = true;
    const auto element = ElementFactory::Create(m_Params.type);
    if (!element) {
        return;
    }
    m_Adaptor->ShowShape(element->BuildShape());
}


void PreviewManager::UpdateElementPreview(const ElementCreateParams &params) const {
    if (!m_IsPreviewState) {
        return;
    }

    GeometryTypes::RTransform transform;
    const auto iter = params.properties.find("LocalTransform");
    if (iter == params.properties.end() || !iter->second.GetValueR(transform)) {
        return;
    }

    m_Adaptor->UpdateTransform(transform);
}


void PreviewManager::ExitPreviewState() {
    m_IsPreviewState = false;
    m_Adaptor->Clear();
}

bool PreviewManager::IsPreviewState() const {
    return m_IsPreviewState;
}
