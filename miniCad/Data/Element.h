//
// Created by ZQD on 2026/5/30.
//

#pragma once
#include <qstring.h>


class Element {
public:
     Element();

     virtual ~Element();

     QString GetName();

protected:
     QString m_Name;
};


