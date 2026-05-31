//
// Created by ZQD on 2026/5/30.
//

#pragma once

#include "Element.h"


class BoxElement : public Element {
public:
    BoxElement();

    [[nodiscard]] double GetWidth() const {
        return m_Width;
    }

    [[nodiscard]] double GetHeight() const {
        return m_Height;
    }

    [[nodiscard]] double GetLength() const {
        return m_Length;
    }

public:
    double m_Width;
    double m_Height;
    double m_Length;
};

