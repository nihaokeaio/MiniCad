//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include <AIS_InteractiveContext.hxx>
#include <memory>


class CadView;
class ViewAdaptor;
class ViewObjectRegistry;
class SelectionManager;
class DocumentObserver;
class Document;

class AppContext {
public:
    AppContext();

    ~AppContext();

public:
    std::unique_ptr<Document> m_Document;
    std::unique_ptr<DocumentObserver> m_DocumentObserver;
    std::unique_ptr<SelectionManager> m_Selection;
    std::unique_ptr<ViewObjectRegistry> m_Registry;
    std::unique_ptr<ViewAdaptor> m_Adaptor;
    CadView *m_View; ///CadView是一个widget，由外部来持有
};


