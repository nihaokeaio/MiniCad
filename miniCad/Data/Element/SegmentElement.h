//
// Created by ZQD on 2026/6/11.
//

#pragma once

#include "Element.h"

class SegmentElement : public Element {
public:
    SegmentElement();

    [[nodiscard]] TopoDS_Shape BuildShape() const override;

    [[nodiscard]] double GetLength() const;
};
