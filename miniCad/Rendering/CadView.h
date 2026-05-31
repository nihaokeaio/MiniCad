#pragma once

#include <QWidget>
#include <Standard_Handle.hxx>

#include "Document.h"
#include "Presentation/SelectionManager.h"
#include "Presentation/ViewObjectRegistry.h"


class SelectionManager;
class ViewObjectRegistry;
class DocumentObserver;
struct ElementId;
class Document;
class Element;
class AIS_InteractiveContext;
class V3d_View;
class V3d_Viewer;

class CadView : public QWidget {
    //Q_OBJECT
public:
    explicit CadView(Document *doc, ViewObjectRegistry *registry, SelectionManager *selectionManager,
                     QWidget *parent = nullptr);

    Handle(AIS_InteractiveContext) GetContext();

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    [[nodiscard]] QPaintEngine *paintEngine() const override;

public:
    void PrintSelection() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    void InitViewer();

    void InitScene() const;

private:
    Handle(V3d_Viewer) m_Viewer;
    Handle(V3d_View) m_View;
    Handle(AIS_InteractiveContext) m_Context;

    QPoint m_LastMousePos;

    Document *m_document;
    ViewObjectRegistry *m_Register;
    SelectionManager *m_SelectionManager;
};
