#ifndef CFIELD_H
#define CFIELD_H

#include "canvdefs.h"
#include "clabel.h"

class CanvasField: public CanvasLabel{
public:
    CanvasField(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasField; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasLabel::updateGeomProps();
    }
};

#endif
