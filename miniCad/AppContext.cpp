//
// Created by ZQD on 2026/5/31.
//

#include "AppContext.h"

#include <qevent.h>
#include "CadView.h"
#include "Document.h"
#include "PreviewAdaptor.h"
#include "Controller/CadController.h"
#include "Controller/Command/CommandManager.h"
#include "Controller/Interaction/CoordinateResolver.h"
#include "Controller/Interaction/InteractionManager.h"
#include "Controller/Preview/PreviewManager.h"
#include "Presentation/ReferenceOverlay.h"
#include "Presentation/SelectionManager.h"
#include "Presentation/DocumentObserver.h"
#include "Presentation/ViewAdaptor.h"
#include "Presentation/ViewObjectRegistry.h"

IMPLEMENT_SINGLETON(AppContext)

AppContext::AppContext() = default;

void AppContext::Initialize() {
    m_Document = std::make_unique<Document>();
    m_Selection = std::make_unique<SelectionManager>();
    m_Registry = std::make_unique<ViewObjectRegistry>();
    m_View = new CadView(m_Document.get(), m_Registry.get(), m_Selection.get());
    m_Adaptor = std::make_unique<ViewAdaptor>(m_View->GetContext(), m_Registry.get(), m_Document.get());
    m_PreviewAdaptor = std::make_unique<PreviewAdaptor>(m_View->GetContext());
    m_ReferenceOverlay = std::make_unique<ReferenceOverlay>(m_View->GetContext());
    m_ReferenceOverlay->ShowGrid(true);
    m_ReferenceOverlay->ShowAxes(true);
    m_CoordinateResolver = std::make_unique<CoordinateResolver>(m_View->GetView());
    m_DocumentObserver = std::make_unique<DocumentObserver>(m_Document.get(), m_Adaptor.get());
    m_CommandManager = std::make_unique<CommandManager>();
    m_CadController = std::make_unique<CadController>(m_Document.get(), m_CommandManager.get());
    m_PreviewManager = std::make_unique<PreviewManager>(m_PreviewAdaptor.get());
    m_InteractionManager = std::make_unique<InteractionManager>(std::make_unique<InteractionContext>(
        m_View->GetContext(),
        m_View->GetView(),
        m_Document.get(),
        m_Registry.get(),
        m_Selection.get(),
        m_CadController.get(),
        m_PreviewManager.get(),
        m_CoordinateResolver.get()
    ));
    m_View->SetInteractionManager(m_InteractionManager.get());
}


Document *AppContext::GetDocument() const {
    return m_Document.get();
}

SelectionManager *AppContext::GetSelectManager() const {
    return m_Selection.get();
}

CadView *AppContext::GetCadView() const {
    return m_View;
}

ViewAdaptor *AppContext::GetViewAdaptor() const {
    return m_Adaptor.get();
}

ViewObjectRegistry *AppContext::GetViewObjectRegistry() const {
    return m_Registry.get();
}

CommandManager *AppContext::GetCommandManager() const {
    return m_CommandManager.get();
}

CadController *AppContext::GetCadController() const {
    return m_CadController.get();
}

InteractionManager *AppContext::GetInteractionManager() const {
    return m_InteractionManager.get();
}

PreviewManager *AppContext::GetPreviewManager() const {
    return m_PreviewManager.get();
}

