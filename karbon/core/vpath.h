#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"
#include "vprimitive.h"

class VPoint;

/**
 * VPaths are the most common high-level objects. They consist of VLines and
 * VBeziers.
 */

class VPath : public VObject {
public:
    VPath();
    virtual ~VPath();
    
    virtual void draw( QPainter& painter );
    
    // postscript-like commands:
    void moveTo( double& x, double& y );
    void lineTo( double& x, double& y );
    void curveTo();
    void close();

private:
    VPoint* m_currentPoint;
    QList<VPoint> m_points;
    QList<VPrimitive> m_primitives;
};

#endif
