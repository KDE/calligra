#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"
#include "vprimitive.h"

class QPainter;
class VPoint;

/**
 * VPaths are the most common high-level objects. They consist of VLines and
 * VBeziers.
 */

class VPath : public VObject {
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
    VPoint* m_currentPoint;
    QList<VPrimitive> m_primitives;
};

#endif
