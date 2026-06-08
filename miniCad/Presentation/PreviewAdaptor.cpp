//
// Created by ZQD on 2026/6/7.
//

#include "PreviewAdaptor.h"

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <gp_Trsf.hxx>

PreviewAdaptor::PreviewAdaptor(const Handle(AIS_InteractiveContext) &context) : m_Context(context) {
}

void PreviewAdaptor::ShowShape(const TopoDS_Shape &shape) {
    if (shape.IsNull()) {
        return;
    }
    if (m_PreviewAis) {
        m_PreviewAis->SetShape(shape);
    } else {
        m_PreviewAis = new AIS_Shape(shape);
        m_Context->Display(m_PreviewAis, AIS_Shaded, -1, Standard_False);
    }
    m_Context->UpdateCurrentViewer();
}

void PreviewAdaptor::UpdateShape(const TopoDS_Shape &shape) {
    ShowShape(shape);
}

void PreviewAdaptor::UpdateTransform(const gp_Trsf &transform) {
    if (m_PreviewAis.IsNull()) {
        return;
    }
    m_PreviewAis->SetLocalTransformation(transform);
    m_Context->UpdateCurrentViewer();
}

void PreviewAdaptor::Clear() {
    if (m_PreviewAis.IsNull()) {
        return;
    }
    m_Context->Remove(m_PreviewAis, Standard_True);
    m_PreviewAis.Nullify();
    m_Context->UpdateCurrentViewer();
}
