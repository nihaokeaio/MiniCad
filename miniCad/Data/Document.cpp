//
// Created by ZQD on 2026/5/31.
//

#include "Document.h"

#include "Element/Element.h"

void Document::RegisterElement(std::unique_ptr<Element> &&element) {
    if (element->GetId() == ElementId::InvalidId) {
        element->SetId(NewElementId());
    }
    auto rawElement = element.get();
    element->SetDocument(this);
    m_Elements.insert(std::make_pair(element->GetId(), std::move(element)));
    rawElement->NotifyElementChanged(MessageInfo::Add);
}

std::unique_ptr<Element> Document::UnregisterElement(const ElementId &elementId) {
    if (const auto it = m_Elements.find(elementId); it != m_Elements.end()) {
        it->second->NotifyElementChanged(MessageInfo::Remove);
        auto element = std::move(it->second);
        m_Elements.erase(it);
        element->SetDocument(nullptr);
        return element;
    }
    return nullptr;
}

ElementId Document::NewElementId() {
    return ElementId(GlobalUniqueId::Instance().NextId());
}

Element *Document::FindElement(const ElementId &elementId) {
    const auto iter = m_Elements.find(elementId);
    if (iter != m_Elements.end()) {
        return iter->second.get();
    }
    return nullptr;
}

void Document::NotifyElementAdded(const ElementId &elementId) const {
    const auto payload = std::make_shared<MessageInfo::ElementChangePayLoad>();
    payload->changeType = MessageInfo::ElementChangeFlag::Add;
    payload->elementId = elementId;
    if (m_NotifyElementChanged) {
        m_NotifyElementChanged(payload);
    }
}

void Document::NotifyElementRemoved(const ElementId &elementId) const {
    const auto payload = std::make_shared<MessageInfo::ElementChangePayLoad>();;
    payload->changeType = MessageInfo::ElementChangeFlag::Remove;
    payload->elementId = elementId;
    if (m_NotifyElementChanged) {
        m_NotifyElementChanged(payload);
    }
}

void Document::NotifyElementUpdated(const ElementId &elementId, MessageInfo::ElementUpdateHint hint) const {
    const auto payload = std::make_shared<MessageInfo::ElementChangePayLoad>();
    payload->changeType = MessageInfo::ElementChangeFlag::Update;
    payload->updateHint = hint;
    payload->elementId = elementId;
    if (m_NotifyElementChanged) {
        m_NotifyElementChanged(payload);
    }
}

void Document::SetNotifyElementChangedCallback(
    const std::function<void(std::shared_ptr<MessageInfo::ElementChangePayLoad>)> &callback) {
    m_NotifyElementChanged = callback;
}
