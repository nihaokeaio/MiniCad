//
// Created by ZQD on 2026/5/31.
//

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <Utils/GlobalUniqueId.h>

#include "Element/ElementId.h"
#include "MessageInfo.h"

class Element;

class Document {
public:
    void RegisterElement(std::unique_ptr<Element> &&element);

    std::unique_ptr<Element> UnregisterElement(const ElementId &elementId);

    static ElementId NewElementId();

    Element *FindElement(const ElementId &elementId);

    template<typename T>
    T *FindElement(const ElementId &elementId);

    void NotifyElementAdded(const ElementId &elementId) const;

    void NotifyElementRemoved(const ElementId &elementId) const;

    void NotifyElementUpdated(const ElementId &elementId,
                              MessageInfo::ElementUpdateHint hint = MessageInfo::ElementUpdateHint::All) const;

    void SetNotifyElementChangedCallback(
        const std::function<void(std::shared_ptr<MessageInfo::ElementChangePayLoad>)> &callback);

private:
    std::unordered_map<ElementId, std::unique_ptr<Element> > m_Elements;
    std::function<void(std::shared_ptr<MessageInfo::ElementChangePayLoad>)> m_NotifyElementChanged;
};

template<typename T>
T *Document::FindElement(const ElementId &elementId) {
    if (const auto iter = m_Elements.find(elementId); iter != m_Elements.end()) {
        return dynamic_cast<T *>(iter->second.get());
    }
    return nullptr;
}
