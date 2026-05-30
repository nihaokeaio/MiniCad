//
// Created by ZQD on 2026/5/30.
//

#include "Element.h"

Element::Element() {
    m_Name = "Element";
}

Element::~Element() {
}

QString Element::GetName() {
    return m_Name;
}
