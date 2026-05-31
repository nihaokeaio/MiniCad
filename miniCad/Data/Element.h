//
// Created by ZQD on 2026/5/30.
//

#pragma once
#include <qstring.h>

#include "Document.h"
#include "ElementId.h"


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

protected:
     QString m_Name;
     ElementId m_Id;
     Document *m_Document;
};


