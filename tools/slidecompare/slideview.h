#ifndef SLIDEVIEW_H
#define SLIDEVIEW_H

#include <QtGui/QWidget>
#include <QtGui/QProgressBar>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

class SlideLoader;
class GraphicsSlideItem;

class SlideView : public QWidget {
friend class GraphicsSlideItem;
private:
    Q_OBJECT
    class SlideGraphicsScene : public QGraphicsScene {
    public:
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    };
    QGraphicsView view;
    SlideGraphicsScene scene;
    QProgressBar progressBar;
    SlideLoader* loader;
    qreal zoomfactor;
    bool sendingChange;

    void layout();
    bool eventFilter(QObject *obj, QEvent *event);
    void toggleSlideZoom(const GraphicsSlideItem* slide);
private slots:
    void slotUpdateSlides();
    void slotViewChanged();
public:
    SlideView(SlideLoader* loader, QWidget* parent = 0);
public slots:
    void setView(qreal zoomFactor, int h, int v);
signals:
    void viewChanged(qreal zoomFactor, int h, int v);
};

#endif
