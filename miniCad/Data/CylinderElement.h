//
// Created by ZQD on 2026/6/3.
//

#pragma once
#include "Element.h"


class CylinderElement : public Element {
public:
    CylinderElement();

    [[nodiscard]] TopoDS_Shape BuildShape() const override;

    [[nodiscard]] double GetRadius() const {
        double value = 0;
        GetProperty("Radius", value);
        return value;
    }

    [[nodiscard]] double GetHeight() const {
        double value = 0;
        GetProperty("Height", value);
        return value;
    }
};
