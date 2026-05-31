//
// Created by ZQD on 2026/5/31.
//

#include "AppContext.h"

#include <qevent.h>
#include "CadView.h"
#include "Document.h"
#include "Presentation/SelectionManager.h"
#include "Presentation/DocumentObserver.h"
#include "Presentation/ViewAdaptor.h"
#include "Presentation/ViewObjectRegistry.h"

AppContext::AppContext() {
    m_Document = std::make_unique<Document>();
    m_Selection = std::make_unique<SelectionManager>();
    m_Registry = std::make_unique<ViewObjectRegistry>();
    m_View = new CadView(m_Document.get(), m_Registry.get(), m_Selection.get());
    m_Adaptor = std::make_unique<ViewAdaptor>(m_View->GetContext(), m_Registry.get(), m_Document.get());
    m_DocumentObserver = std::make_unique<DocumentObserver>(m_Document.get(), m_Adaptor.get());
}

AppContext::~AppContext() = default;
