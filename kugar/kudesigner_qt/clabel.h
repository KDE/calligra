#ifndef CLABEL_H
#define CLABEL_H

#include "creportitem.h"
#include "canvdefs.h"

class CanvasLabel: public CanvasReportItem{
public:
    CanvasLabel(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasLabel; }
    virtual QString getXml();
    
    int getTextAlignment();
    int getTextWrap();
    
    QFont getFont();
    QPen getPenForText();
    QPen getPenForShape();
    QBrush getBrush();
    
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasReportItem::updateGeomProps();
    }
};

#endif
