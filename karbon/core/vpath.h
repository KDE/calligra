#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"
#include "vprimitive.h"

// TODO: moveTo inline ?

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
    void rmoveTo( double& x, double& y );    
    void lineTo( double& x, double& y );
    void rlineTo( double& x, double& y );
    void curveTo( double& x1, double& y1, double& x2, double& y2, double& x3, double& y3 );
    void rcurveTo();    
    void close();

private:
    QList<VPoint> m_points;		// list of used points
    QList<VPrimitive> m_primitives;	// list of used primitives
};

#endif
