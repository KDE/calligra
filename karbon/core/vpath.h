#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"
#include "vprimitive.h"

class VPainter;
class VPoint;

class VPath : public VObject {
public:
    VPath();
    virtual ~VPath();
    
    virtual void draw( VPainter& p );
    
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
