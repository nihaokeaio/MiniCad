//
// Created by ZQD on 2026/6/16.
//

#include "ViewStateAdaptor.h"

#include "SelectionManager.h"
#include "ViewObjectRegistry.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include "TransformGuideAdaptor.h"

ViewStateAdaptor::ViewStateAdaptor(const Handle(AIS_InteractiveContext) &context, ViewObjectRegistry *registry)
    : m_Context(context),
      m_Registry(registry) {
}

ViewStateAdaptor::~ViewStateAdaptor() = default;

void ViewStateAdaptor::ShowPreviewShape(const TopoDS_Shape &shape) {
    if (shape.IsNull()) {
        return;
    }

    if (m_PreviewAis.IsNull()) {
        m_PreviewAis = new AIS_Shape(shape);
        m_Context->Display(m_PreviewAis, AIS_Shaded, -1, Standard_False);
    } else {
        m_PreviewAis->SetShape(shape);
    }
    m_Context->UpdateCurrentViewer();
}

void ViewStateAdaptor::UpdatePreviewShape(const TopoDS_Shape &shape) {
    ShowPreviewShape(shape);
}

void ViewStateAdaptor::UpdatePreviewTransform(const gp_Trsf &transform) {
    if (m_PreviewAis.IsNull()) {
        return;
    }

    m_PreviewAis->SetLocalTransformation(transform);
    m_Context->UpdateCurrentViewer();
}

void ViewStateAdaptor::ClearPreview() {
    if (m_PreviewAis.IsNull()) {
        return;
    }

    m_Context->Remove(m_PreviewAis, Standard_False);
    m_PreviewAis.Nullify();
    m_Context->UpdateCurrentViewer();
}

void ViewStateAdaptor::ApplySelection(const SelectionManager &selectionManager) const {
    ClearSelection(false);
    if (m_Registry == nullptr) {
        m_Context->UpdateCurrentViewer();
        return;
    }

    for (const auto &target: selectionManager.SelectedTargets()) {
        for (const auto &aisObject: m_Registry->FindElementAisObjects(target.elementId)) {
            if (!aisObject.IsNull()) {
                m_Context->SetSelected(aisObject, Standard_False);
            }
        }
    }

    m_Context->UpdateCurrentViewer();
}

void ViewStateAdaptor::ClearSelection() const {
    ClearSelection(true);
}

void ViewStateAdaptor::ClearSelection(bool updateViewer) const {
    m_Context->ClearSelected(Standard_False);
    if (updateViewer) {
        m_Context->UpdateCurrentViewer();
    }
}

void ViewStateAdaptor::ApplyElementTransforms(const std::vector<ElementViewTransform> &transforms) {
    if (!m_TransformPreviewAdaptor) {
        m_TransformPreviewAdaptor = std::make_unique<TransformPreviewAdaptor>(m_Context, m_Registry);
    }
    m_TransformPreviewAdaptor->Apply(transforms);
}

void ViewStateAdaptor::ShowTransformGuide(const std::shared_ptr<TransformGuideState> &transformGuideState) {
    if (!m_TransformGuideAdaptor) {
        m_TransformGuideAdaptor = std::make_unique<TransformGuideAdaptor>(m_Context);
    }
    m_TransformGuideAdaptor->ShowTransformGuide(transformGuideState);
}
