#ifndef CSPECIALFIELD_H
#define CSPECIALFIELD_H

#include "canvdefs.h"
#include "clabel.h"

class CanvasSpecialField: public CanvasLabel{
public:
    CanvasSpecialField(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiCanvasSpecial; }
    virtual QString getXml();
    virtual void draw(QPainter &painter);
    virtual void updateGeomProps()
    {
	CanvasLabel::updateGeomProps();
    }
};

#endif
