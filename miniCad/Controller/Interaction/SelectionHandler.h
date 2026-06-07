//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include "InteractionHandler.h"

class SelectionHandler : public InteractionHandler {
public:
    explicit SelectionHandler(InteractionContext *context);

    bool MouseRelease(QMouseEvent *event) override;
};
