#ifndef SKETCHDECLARATIVEVIEW_H
#define SKETCHDECLARATIVEVIEW_H

#include <QDeclarativeView>
/**
 * @brief The SketchDeclarativeView class overrides QGraphicsView's drawBackground
 */
class SketchDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool drawCanvas READ drawCanvas WRITE setDrawCanvas NOTIFY drawCanvasChanged);

public:
    SketchDeclarativeView(QWidget *parent = 0);
    SketchDeclarativeView(const QUrl &url, QWidget *parent = 0);
    virtual ~SketchDeclarativeView();

    void setCanvasWidget(QWidget *canvasWidget);

    bool drawCanvas() const;
    void setDrawCanvas(bool drawCanvas);

signals:

    void drawCanvasChanged();

protected:

    void drawBackground(QPainter *painter, const QRectF &rect);
private:

    bool m_drawCanvas;
    QWidget *m_canvasWidget;
};

#endif // SKETCHDECLARATIVEVIEW_H
