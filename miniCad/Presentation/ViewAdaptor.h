//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include <qwindowdefs_win.h>

#include "../Data/Element/Element.h"
#include "ViewObjectRegistry.h"


class ViewAdaptor {
public:
    explicit ViewAdaptor(const opencascade::handle<AIS_InteractiveContext> &context, ViewObjectRegistry *registry,
                         Document *doc);

    void AddElement(ElementId elementId) const;

    void RemoveElement(ElementId id) const;

    void UpdateElement(const std::shared_ptr<MessageInfo::ElementChangePayLoad> &payload) const;

private:
    Handle(AIS_InteractiveContext) m_Context;
    ViewObjectRegistry *m_Registry;
    Document *m_Document;
};


