//
// Created by ZQD on 2026/5/31.
//

#include "ViewAdaptor.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include "BoxElement.h"

ViewAdaptor::ViewAdaptor(const opencascade::handle<AIS_InteractiveContext> &context, ViewObjectRegistry *registry,
                         Document *doc) : m_Context(context), m_Registry(registry), m_Document(doc) {
}

void ViewAdaptor::AddElement(ElementId elementId) const {
    const auto box = m_Document->FindElement<BoxElement>(elementId);
    if (!box)
        return;


    const TopoDS_Shape shape = BRepPrimAPI_MakeBox(box->GetLength(), box->GetWidth(), box->GetHeight());
    Handle(AIS_Shape) ais = new AIS_Shape(shape);
    m_Context->Display(ais, AIS_Shaded, 0, Standard_True);
    m_Registry->Register(ais, elementId);
}

void ViewAdaptor::RemoveElement(ElementId id) const {
    assert(id!=ElementId::InvalidId);
    for (auto aisObject: m_Registry->FindElementAisObjects(id)) {
        m_Context->Remove(aisObject, false);
    }
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
        default: {
        }
    }
}

