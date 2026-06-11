//
// Created by ZQD on 2026/6/11.
//

#pragma once

#include "Element.h"

class PointElement : public Element {
public:
    PointElement();

    [[nodiscard]] TopoDS_Shape BuildShape() const override;
};
