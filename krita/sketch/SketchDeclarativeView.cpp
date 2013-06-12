#include "SketchDeclarativeView.h"
#include <QWidget>
#include <QDebug>

SketchDeclarativeView::SketchDeclarativeView(QWidget *parent)
    : QDeclarativeView(parent)
    , m_canvasWidget(0)
{
    setCacheMode(QGraphicsView::CacheNone);
}

SketchDeclarativeView::SketchDeclarativeView(const QUrl &url, QWidget *parent)
    : QDeclarativeView(url, parent)
    , m_canvasWidget(0)
{
    setCacheMode(QGraphicsView::CacheNone);
}

SketchDeclarativeView::~SketchDeclarativeView()
{
    m_canvasWidget = 0;
}

void SketchDeclarativeView::setCanvasWidget(QWidget *canvasWidget)
{
    m_canvasWidget = canvasWidget;
}

bool SketchDeclarativeView::drawCanvas() const
{
    return m_drawCanvas;
}

void SketchDeclarativeView::setDrawCanvas(bool drawCanvas)
{
    if (m_drawCanvas != drawCanvas) {
        m_drawCanvas = drawCanvas;
        emit drawCanvasChanged();
    }
}

void SketchDeclarativeView::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (m_drawCanvas && m_canvasWidget) {
        m_canvasWidget->render(painter);
    }
    QDeclarativeView::drawBackground(painter, rect);

}
