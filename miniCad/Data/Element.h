//
// Created by ZQD on 2026/5/30.
//

#pragma once
#include <TopoDS_Shape.hxx>
#include <qstring.h>

#include "Document.h"
#include "ElementId.h"
#include "Property/PropertySet.h"


class Element {
public:
     Element();

     [[nodiscard]] Document *GetDocument() const;

     void SetDocument(Document *doc);

     void NotifyElementChanged(MessageInfo::ElementChangeFlag flag) const;

     [[nodiscard]] ElementId GetId() const;

     void SetId(const ElementId &elementId);

     virtual ~Element();

     QString GetName();

     [[nodiscard]] virtual TopoDS_Shape BuildShape() const = 0;

     [[nodiscard]] PropertySet &Properties();

     [[nodiscard]] const PropertySet &Properties() const;

     void SetProperty(const std::string &key, const PropertyValue &value);

     template<typename T>
     void SetPropertyT(const std::string &key, const T &value);

     template<typename T>
     bool GetProperty(const std::string &key, T &value) const;

protected:
     QString m_Name;
     ElementId m_Id;
     Document *m_Document;
     PropertySet m_Properties;
};

template<typename T>
void Element::SetPropertyT(const std::string &key, const T &value) {
     SetProperty(key, PropertyValue(value));
}

template<typename T>
bool Element::GetProperty(const std::string &key, T &value) const {
     return m_Properties.Get(key, value);
}

