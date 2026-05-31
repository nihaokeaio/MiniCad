#pragma once

#include <QWidget>
#include <Standard_Handle.hxx>


struct ElementId;
class Document;
class Element;
class AIS_InteractiveContext;
class V3d_View;
class V3d_Viewer;

class CadView : public QWidget {
    //Q_OBJECT
public:
    explicit CadView(QWidget *parent = nullptr);

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

    void InitScene();

private:
    Handle(V3d_Viewer) mViewer;
    Handle(V3d_View) mView;
    Handle(AIS_InteractiveContext) mContext;

    QPoint mLastMousePos;
    std::unique_ptr<Document> m_document;
    std::unordered_map<void *, ElementId> m_ObjectMap;
};
