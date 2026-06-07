//
// Created by ZQD on 2026/6/3.
//

#pragma once
#include "Element.h"


class CylinderElement : public Element {
public:
    CylinderElement();

    [[nodiscard]] TopoDS_Shape BuildShape() const override;

    [[nodiscard]] double GetRadius() const;

    [[nodiscard]] double GetHeight() const;
};
