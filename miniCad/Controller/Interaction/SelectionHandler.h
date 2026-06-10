//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <memory>

#include "InteractionHandler.h"

class ScenePicker;

class SelectionHandler : public InteractionHandler {
public:
    explicit SelectionHandler(InteractionContext *context);

    bool MousePress(QMouseEvent *event) override;

    bool MouseRelease(QMouseEvent *event) override;

private:
    std::unique_ptr<ScenePicker> m_ScenePicker;
};
