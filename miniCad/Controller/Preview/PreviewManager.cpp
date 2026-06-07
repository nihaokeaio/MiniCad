//
// Created by ZQD on 2026/6/7.
//

#include "PreviewManager.h"

#include "Element.h"
#include "ElementFactory.h"
#include "GeometryTypes.h"
#include "Presentation/PreviewAdaptor.h"

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

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

    GeometryTypes::Point3D position(0.0, 0.0, 0.0);
    const auto iter = params.properties.find("Position");
    if (iter == params.properties.end() || !iter->second.GetValueR(position)) {
        return;
    }

    gp_Trsf transform;
    transform.SetTranslation(gp_Vec(position));
    m_Adaptor->UpdateTransform(transform);
}


void PreviewManager::ExitPreviewState() {
    m_IsPreviewState = false;
    m_Adaptor->Clear();
}

bool PreviewManager::IsPreviewState() const {
    return m_IsPreviewState;
}
