//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include "ViewAdaptor.h"


class ViewAdaptor;
class Document;
class Scene;

class DocumentObserver {
public:
    explicit DocumentObserver(Document *doc, ViewAdaptor *viewAdaptor, Scene *scene = nullptr);

private:
    ViewAdaptor *m_ViewAdaptor;
    Scene *m_Scene;
};


