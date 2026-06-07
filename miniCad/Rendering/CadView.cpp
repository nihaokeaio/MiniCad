#include "CadView.h"
#include "Document.h"
#include "BoxElement.h"
#include "Controller/Interaction/InteractionManager.h"
#include "Presentation/SelectionManager.h"
#include "Presentation/ViewObjectRegistry.h"

#include <QMouseEvent>
#include <WNT_Window.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_InteractiveContext.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>
#include <QTimer>
#include <QDebug>


CadView::CadView(Document *doc, ViewObjectRegistry *registry, SelectionManager *selectionManager,
                 QWidget *parent) : m_document(doc), m_Register(registry), m_SelectionManager(selectionManager),
                                    QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAutoFillBackground(false);
    setMouseTracking(true);
    InitViewer();
    InitScene();
    QTimer::singleShot(0, this, [this]() {
        if (!m_View.IsNull()) {
            m_View->MustBeResized();
            m_View->FitAll();
            m_View->Redraw();
        }
    });
}

void CadView::InitViewer() {
    Handle(Aspect_DisplayConnection) display = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(display);
    m_Viewer = new V3d_Viewer(driver);
    m_Viewer->SetDefaultLights();
    m_Viewer->SetLightOn();

    m_Context = new AIS_InteractiveContext(m_Viewer);

    m_View = m_Viewer->CreateView();

    Handle(WNT_Window) window = new WNT_Window(reinterpret_cast<Aspect_Handle>(winId()));

    m_View->SetWindow(window);

    if (!window->IsMapped())
        window->Map();

    m_View->SetBackgroundColor(Quantity_NOC_GRAY30);
    m_View->MustBeResized();
}

void CadView::InitScene() const {
    m_View->SetComputedMode(false);
    m_Context->DefaultDrawer()->SetFaceBoundaryDraw(true);
    m_View->FitAll();
}


opencascade::handle<AIS_InteractiveContext> CadView::GetContext() {
    return m_Context;
}

opencascade::handle<V3d_View> CadView::GetView() {
    return m_View;
}

void CadView::SetInteractionManager(InteractionManager *interactionManager) {
    m_InteractionManager = interactionManager;
}


void CadView::paintEvent(QPaintEvent *) {
    if (!m_View.IsNull()) {
        m_View->Redraw();
    }
}

QPaintEngine *CadView::paintEngine() const {
    return nullptr;
}

void CadView::PrintSelection() const {
    for (m_Context->InitSelected(); m_Context->MoreSelected(); m_Context->NextSelected()) {
        auto obj = m_Context->SelectedInteractive();
        if (!obj.IsNull()) {
            const auto elementId = m_Register->FindElement(obj.get());
            if (const auto element = m_document->FindElement(elementId)) {
                qDebug() << "selected object " << element->GetName();
            } else {
                qDebug() << "selected  Null!";
            }
        }
    }
}

void CadView::resizeEvent(QResizeEvent *) {
    if (!m_View.IsNull()) {
        m_View->MustBeResized();
    }
}

void CadView::mousePressEvent(QMouseEvent *event) {
    if (m_InteractionManager) {
        m_InteractionManager->MousePress(event);
    }
    QWidget::mousePressEvent(event);
}

void CadView::mouseReleaseEvent(QMouseEvent *event) {
    if (m_InteractionManager) {
        m_InteractionManager->MouseRelease(event);
    }
}

void CadView::mouseMoveEvent(QMouseEvent *event) {
    if (m_InteractionManager) {
        m_InteractionManager->MouseMove(event);
    }
}

void CadView::wheelEvent(QWheelEvent *event) {
    if (m_InteractionManager) {
        m_InteractionManager->Wheel(event);
    }
}
