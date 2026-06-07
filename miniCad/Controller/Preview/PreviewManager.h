//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <vector>

#include "../ElementCreateParams.h"

class PreviewManager {
public:
    void BeginElementPreview(const ElementCreateParams &params);

    void UpdateElementPreviewPosition(const std::vector<double> &position);

    void Clear();

    [[nodiscard]] bool HasPreview() const;

private:
    bool m_HasPreview = false;
    ElementCreateParams m_CurrentParams{ElementType::Box, {}};
};
