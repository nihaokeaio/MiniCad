//
// Created by ZQD on 2026/5/31.
//

#include "ViewAdaptor.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>

#include "Element.h"

ViewAdaptor::ViewAdaptor(const opencascade::handle<AIS_InteractiveContext> &context, ViewObjectRegistry *registry,
                         Document *doc) : m_Context(context), m_Registry(registry), m_Document(doc) {
}

void ViewAdaptor::AddElement(ElementId elementId) const {
    const auto element = m_Document->FindElement(elementId);
    if (!element) {
        return;
    }

    const TopoDS_Shape shape = element->BuildShape();
    if (shape.IsNull()) {
        return;
    }
    Handle(AIS_Shape) ais = new AIS_Shape(shape);
    m_Context->Display(ais, AIS_Shaded, 0, Standard_True);
    m_Registry->Register(ais, elementId);
}

void ViewAdaptor::RemoveElement(ElementId id) const {
    assert(id!=ElementId::InvalidId);
    for (auto aisObject: m_Registry->FindElementAisObjects(id)) {
        m_Context->Remove(aisObject, Standard_False);
    }
    m_Context->UpdateCurrentViewer();
    m_Registry->UnRegister(id);
}

void ViewAdaptor::UpdateElement(const std::shared_ptr<MessageInfo::ElementChangePayLoad> &payload) const {
    switch (payload->changeType) {
        case MessageInfo::ElementChangeFlag::Add: {
            AddElement(payload->elementId);
            break;
        }
        case MessageInfo::ElementChangeFlag::Remove: {
            RemoveElement(payload->elementId);
            break;
        }
        case MessageInfo::ElementChangeFlag::Update: {
            RemoveElement(payload->elementId);
            AddElement(payload->elementId);
            break;
        }
        default: {
        }
    }
}

