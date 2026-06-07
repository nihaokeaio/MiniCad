//
// Created by ZQD on 2026/5/30.
//

#include "Element.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

Element::Element() : m_Id(ElementId::InvalidId), m_Document(nullptr) {
    m_Name = "Element";
    m_Properties.SetT("Position", std::vector<double>{0.0, 0.0, 0.0});
}

Document *Element::GetDocument() const {
    return m_Document;
}

void Element::SetDocument(Document *doc) {
    m_Document = doc;
}

void Element::NotifyElementChanged(MessageInfo::ElementChangeFlag flag) const {
    assert(m_Document != nullptr);
    switch (flag) {
        case MessageInfo::ElementChangeFlag::Add:
            m_Document->NotifyElementAdded(m_Id);
            break;
        case MessageInfo::ElementChangeFlag::Remove:
            m_Document->NotifyElementRemoved(m_Id);
            break;
        default:
            m_Document->NotifyElementUpdated(m_Id);
    }
}

ElementId Element::GetId() const {
    return m_Id;
}

void Element::SetId(const ElementId &elementId) {
    m_Id = elementId;
}

Element::~Element() = default;

QString Element::GetName() {
    return m_Name;
}

PropertySet &Element::Properties() {
    return m_Properties;
}

const PropertySet &Element::Properties() const {
    return m_Properties;
}

void Element::SetProperty(const std::string &key, const PropertyValue &value) {
    m_Properties.Set(key, value);
    if (m_Document != nullptr) {
        NotifyElementChanged(MessageInfo::ElementChangeFlag::Update);
    }
}

void Element::GetProperty(const std::string &key, PropertyValue &value) const {
    if (m_Properties.Exists(key)) {
        value = m_Properties.Get(key);
    }
}

std::vector<double> Element::GetPosition() const {
    std::vector<double> position{0.0, 0.0, 0.0};
    GetProperty("Position", position);
    if (position.size() != 3) {
        return {0.0, 0.0, 0.0};
    }
    return position;
}

TopoDS_Shape Element::ApplyPlacement(const TopoDS_Shape &shape) const {
    const auto position = GetPosition();
    if (position[0] == 0.0 && position[1] == 0.0 && position[2] == 0.0) {
        return shape;
    }

    gp_Trsf transform;
    transform.SetTranslation(gp_Vec(position[0], position[1], position[2]));
    return BRepBuilderAPI_Transform(shape, transform).Shape();
}
