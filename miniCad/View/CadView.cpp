#include "CadView.h"
#include "Document.h"
#include "BoxElement.h"

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


CadView::CadView(QWidget *parent)
    : QWidget(parent), m_document(std::make_unique<Document>()) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAutoFillBackground(false);
    setMouseTracking(true);
    InitViewer();
    InitScene();
    QTimer::singleShot(0, this, [this]() {
        if (!mView.IsNull()) {
            mView->MustBeResized();
            mView->FitAll();
            mView->Redraw();
        }
    });
}

void CadView::InitViewer() {
    Handle(Aspect_DisplayConnection) display = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(display);
    mViewer = new V3d_Viewer(driver);
    mViewer->SetDefaultLights();
    mViewer->SetLightOn();

    mContext = new AIS_InteractiveContext(mViewer);

    mView = mViewer->CreateView();

    Handle(WNT_Window) window = new WNT_Window(reinterpret_cast<Aspect_Handle>(winId()));

    mView->SetWindow(window);

    if (!window->IsMapped())
        window->Map();

    mView->SetBackgroundColor(Quantity_NOC_GRAY30);
    mView->MustBeResized();
}

void CadView::InitScene() {
    TopoDS_Shape shape = BRepPrimAPI_MakeBox(100, 100, 100);
    Handle(AIS_Shape) box = new AIS_Shape(shape);
    mView->SetComputedMode(false);
    mContext->DefaultDrawer()->SetFaceBoundaryDraw(true);
    auto boxElement = std::make_unique<BoxElement>();
    boxElement->SetId(Document::NewElementId());
    m_ObjectMap.insert({box.get(), boxElement->GetId()});
    m_document->RegisterElement(std::move(boxElement));
    mContext->Display(box, AIS_Shaded, 0, Standard_True);

    mView->FitAll();
}

void CadView::paintEvent(QPaintEvent *) {
    if (!mView.IsNull()) {
        mView->Redraw();
    }
}

QPaintEngine *CadView::paintEngine() const {
    return nullptr;
}

void CadView::PrintSelection() const {
    for (mContext->InitSelected(); mContext->MoreSelected(); mContext->NextSelected()) {
        auto obj = mContext->SelectedInteractive();
        if (!obj.IsNull()) {
            const auto iter = m_ObjectMap.find(obj.get());
            if (const auto element = m_document->FindElement(iter->second)) {
                qDebug() << "selected object " << element->GetName();
            } else {
                qDebug() << "selected  Null!";
            }
        }
    }
}

void CadView::resizeEvent(QResizeEvent *) {
    if (!mView.IsNull()) {
        mView->MustBeResized();
    }
}

void CadView::mousePressEvent(
    QMouseEvent *event) {
    mLastMousePos = event->pos();
    if (event->button() == Qt::LeftButton) {
        mView->StartRotation(event->x(), event->y());
    }
    QWidget::mousePressEvent(event);
}

void CadView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton)
        return;

    if (!mContext->HasDetected()) {
        mContext->ClearSelected(Standard_True);
        return;
    }
    mContext->SelectDetected();
}

void CadView::mouseMoveEvent(
    QMouseEvent *event) {
    QPoint delta = event->pos() - mLastMousePos;

    if (event->buttons() & Qt::LeftButton) {
        mView->Rotation(event->x(), event->y());
    }

    if (event->buttons() & Qt::MiddleButton) {
        mView->Pan(delta.x(), -delta.y());
    }

    mLastMousePos = event->pos();

    mContext->MoveTo(event->x(), event->y(), mView,Standard_True);
}

void CadView::wheelEvent(
    QWheelEvent *event) {
    const double factor = event->angleDelta().y() < 0 ? 0.9 : 1.1;
    mView->SetScale(mView->Scale() * factor);
}
