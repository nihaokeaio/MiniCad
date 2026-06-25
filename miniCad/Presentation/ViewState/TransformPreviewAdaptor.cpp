//
// Created by ZQD on 2026/6/17.
//

#include "TransformPreviewAdaptor.h"

#include "ViewObjectRegistry.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>

TransformPreviewAdaptor::TransformPreviewAdaptor(const Handle(AIS_InteractiveContext) &context,
                                                 ViewObjectRegistry *registry)
    : m_Context(context), m_Registry(registry) {
}

void TransformPreviewAdaptor::Apply(const std::vector<ElementViewTransform> &transforms) const {
    if (m_Registry == nullptr) {
        return;
    }

    for (const auto &item: transforms) {
        if (!item.elementId.IsValid()) {
            continue;
        }

        for (const auto &aisObject: m_Registry->FindElementAisObjects(item.elementId)) {
            if (!aisObject.IsNull()) {
                aisObject->SetLocalTransformation(item.transform);
                m_Context->Redisplay(aisObject, Standard_False);
            }
        }
    }

    m_Context->UpdateCurrentViewer();
}
