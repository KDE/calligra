#ifndef CV_H
#define CV_H

#include <qcanvas.h>

class CanvasReportItem;
class QMouseEvent;

class ReportCanvas: public QCanvasView{
    Q_OBJECT
public:
    ReportCanvas(QCanvas * canvas, QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    
    CanvasReportItem *selectedItem;
protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
private:
    CanvasReportItem *moving;
    QPoint moving_start;
    CanvasReportItem *resizing;
};

#endif
