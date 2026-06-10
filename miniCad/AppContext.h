//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include <AIS_InteractiveContext.hxx>
#include <memory>

#include "MacroSet.h"


class PreviewAdaptor;
class CoordinateResolver;
class InteractionManager;
class CommandManager;
class CadController;
class PreviewManager;
class ReferenceOverlay;
class CadView;
class ViewAdaptor;
class ViewObjectRegistry;
class SelectionManager;
class DocumentObserver;
class Document;
class Scene;

class AppContext {
    DECLARE_SINGLETON(AppContext)

public:
    void Initialize();

public:
    [[nodiscard]] Document *GetDocument() const;

    [[nodiscard]] SelectionManager *GetSelectManager() const;

    [[nodiscard]] CadView *GetCadView() const;

    [[nodiscard]] ViewAdaptor *GetViewAdaptor() const;

    [[nodiscard]] ViewObjectRegistry *GetViewObjectRegistry() const;

    [[nodiscard]] CommandManager *GetCommandManager() const;

    [[nodiscard]] CadController *GetCadController() const;

    [[nodiscard]] InteractionManager *GetInteractionManager() const;

    [[nodiscard]] PreviewManager *GetPreviewManager() const;

    [[nodiscard]] Scene *GetScene() const;

private:
    std::unique_ptr<Document> m_Document;
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<DocumentObserver> m_DocumentObserver;
    std::unique_ptr<SelectionManager> m_Selection;
    std::unique_ptr<ViewObjectRegistry> m_Registry;
    std::unique_ptr<ViewAdaptor> m_Adaptor;
    std::unique_ptr<PreviewAdaptor> m_PreviewAdaptor;
    std::unique_ptr<ReferenceOverlay> m_ReferenceOverlay;
    std::unique_ptr<CoordinateResolver> m_CoordinateResolver;
    std::unique_ptr<CommandManager> m_CommandManager;
    std::unique_ptr<PreviewManager> m_PreviewManager;
    std::unique_ptr<InteractionManager> m_InteractionManager;
    std::unique_ptr<CadController> m_CadController;

    CadView *m_View = nullptr;
};


