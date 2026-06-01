//
// Created by ZQD on 2026/6/1.
//

#include "Command.h"

#include <cassert>

Command::Command(Document *document) : m_Document(document) {
    assert(m_Document != nullptr);
}

Command::~Command() = default;
