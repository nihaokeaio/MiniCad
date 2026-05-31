//
// Created by ZQD on 2026/5/31.
//

#include "DocumentObserver.h"

#include <assert.h>

#include "Document.h"
#include "ViewAdaptor.h"

DocumentObserver::DocumentObserver(Document *doc, ViewAdaptor *viewAdaptor) {
    assert(doc != nullptr);
    m_ViewAdaptor = viewAdaptor;
    doc->SetNotifyElementChangedCallback([this](const std::shared_ptr<MessageInfo::ElementChangePayLoad> &payload) {
        m_ViewAdaptor->UpdateElement(payload);
    });
}
