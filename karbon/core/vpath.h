#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vdrawable.h"
#include "vprimitive.h"

class QPainter;
class VPoint;

class VPath : public VDrawable {
public:
    VPath();
    virtual ~VPath();
    
    virtual void draw( QPainter& p );
    
    // postscript-like commands:
    void moveTo();
    void lineTo();
    void curveTo();
    void close();

private:
    VPoint m_curPoint;
    QList<VPrimitive> m_primitives;
};

#endif
