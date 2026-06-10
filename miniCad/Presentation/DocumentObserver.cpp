//
// Created by ZQD on 2026/5/31.
//

#include "DocumentObserver.h"

#include <assert.h>

#include "Document.h"
#include "../Data/Element/Element.h"
#include "Scene/Scene.h"
#include "ViewAdaptor.h"

DocumentObserver::DocumentObserver(Document *doc, ViewAdaptor *viewAdaptor, Scene *scene) {
    assert(doc != nullptr);
    m_ViewAdaptor = viewAdaptor;
    m_Scene = scene;
    doc->SetNotifyElementChangedCallback(
        [this, doc](const std::shared_ptr<MessageInfo::ElementChangePayLoad> &payload) {
            if (m_Scene != nullptr) {
                switch (payload->changeType) {
                    case MessageInfo::ElementChangeFlag::Add:
                    case MessageInfo::ElementChangeFlag::Update:
                        if (const auto element = doc->FindElement(payload->elementId)) {
                            m_Scene->AddOrUpdate(*element);
                        }
                        break;
                    case MessageInfo::ElementChangeFlag::Remove:
                        m_Scene->Remove(payload->elementId);
                        break;
                }
            }
            m_ViewAdaptor->UpdateElement(payload);
        });
}
