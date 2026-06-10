//
// Created by ZQD on 2026/5/30.
//

#include "Element.h"

#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>

Element::Element() : m_Id(ElementId::InvalidId), m_Document(nullptr) {
    m_Name = "Element";
    m_Properties.SetT("LocalTransform", GeometryTypes::RTransform());
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

std::string Element::GetName() {
    return m_Name;
}

Bnd_Box Element::GetBoundingBox() const {
    Bnd_Box box;
    const auto shape = ApplyPlacement(BuildShape());
    if (!shape.IsNull()) {
        BRepBndLib::Add(shape, box);
    }
    return box;
}

PropertySet &Element::Properties() {
    return m_Properties;
}

const PropertySet &Element::Properties() const {
    return m_Properties;
}

void Element::SetProperty(const std::string &key, const PropertyValue &value) {
    m_Properties.Set(key, value);
    const auto hint = OnSetProperty(key, value);
    if (m_Document != nullptr) {
        m_Document->NotifyElementUpdated(m_Id, hint);
    }
}

void Element::GetProperty(const std::string &key, PropertyValue &value) const {
    if (m_Properties.Exists(key)) {
        value = m_Properties.Get(key);
    }
}

TopoDS_Shape Element::ApplyPlacement(const TopoDS_Shape &shape) const {
    const auto transform = GetLocalTransform();
    if (transform.Form() == gp_Identity) {
        return shape;
    }

    return BRepBuilderAPI_Transform(shape, transform).Shape();
}

GeometryTypes::RTransform Element::GetLocalTransform() const {
    GeometryTypes::RTransform transform;
    GetProperty("LocalTransform", transform);
    return transform;
}

MessageInfo::ElementUpdateHint Element::OnSetProperty(const std::string &key, const PropertyValue &) {
    if (key == "LocalTransform") {
        return MessageInfo::ElementUpdateHint::Transform;
    }
    return MessageInfo::ElementUpdateHint::Geometry;
}
