//
// Created by ZQD on 2026/6/9.
//

#include "ViewController.h"

#include <Graphic3d_Camera.hxx>
#include <V3d_View.hxx>

#include "Document.h"
#include "Element.h"
#include "InteractionManager.h"
#include "SelectionManager.h"

ViewController::ViewController(InteractionContext *interactionContext) : m_InteractionContext(interactionContext) {
    SetOrthographic();
}

void ViewController::FitAll() const {
    m_InteractionContext->m_View->FitAll();
    m_InteractionContext->m_View->Redraw();
}

void ViewController::FocusSelection() const {
    const auto elementId = m_InteractionContext->m_Selection->GetSingleSelected();
    if (!elementId.IsValid()) {
        FitAll();
        return;
    }
    FocusElement(elementId);
}

void ViewController::FocusElement(const ElementId id) const {
    if (const auto element = m_InteractionContext->m_Document->FindElement(id)) {
        const auto box = element->GetBoundingBox();
        if (box.IsVoid()) {
            FitAll();
            return;
        }
        m_InteractionContext->m_View->FitAll(box);
        m_InteractionContext->m_View->Redraw();
        return;
    }
    FitAll();
}

void ViewController::SetOrthographic() const {
    m_InteractionContext->m_View->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
    m_InteractionContext->m_View->Redraw();
}

void ViewController::SetPerspective() const {
    m_InteractionContext->m_View->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
    m_InteractionContext->m_View->Redraw();
}

void ViewController::SetAxoView() const {
    SetOrthographic();
    m_InteractionContext->m_View->SetProj(V3d_TypeOfOrientation_Zup_AxoRight);
    m_InteractionContext->m_View->Redraw();
}

void ViewController::SetTopView() const {
    SetOrthographic();
    m_InteractionContext->m_View->SetProj(V3d_TypeOfOrientation_Zup_Top);
    m_InteractionContext->m_View->Redraw();
}
