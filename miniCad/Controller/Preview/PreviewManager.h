//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <vector>

#include "../ElementCreateParams.h"

class ViewStateAdaptor;

class PreviewManager {
public:
    explicit PreviewManager(ViewStateAdaptor *adaptor);

    void BeginElementPreview(const ElementCreateParams &params);

    void UpdateElementPreview(const ElementCreateParams &params) const;

    void ExitPreviewState();

    [[nodiscard]] bool IsPreviewState() const;

private:
    bool m_IsPreviewState = false;
    ViewStateAdaptor *m_Adaptor = nullptr;
    ElementCreateParams m_Params{ElementType::Box, {}};
};
