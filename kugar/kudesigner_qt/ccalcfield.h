#ifndef CCALCFIELD_H
#define CCALCFIELD_H

#include "cfield.h"
#include "canvdefs.h"

class CanvasCalculatedField: public CanvasField{
public:
    CanvasCalculatedField(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasCalculated; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasField::updateGeomProps();
    }
};

#endif
