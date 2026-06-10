//
// Created by ZQD on 2026/5/30.
//

#pragma once

#include "Element.h"


class BoxElement : public Element {
public:
    BoxElement();

    [[nodiscard]] TopoDS_Shape BuildShape() const override;

    [[nodiscard]] double GetWidth() const;

    [[nodiscard]] double GetHeight() const;

    [[nodiscard]] double GetLength() const;
};
