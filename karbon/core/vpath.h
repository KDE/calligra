#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"

class VPoint;

/**
 * VPaths are the most common high-level objects. They consist of VLines and
 * VBeziers.
 */

// TODO: refine moveto-behaviour (should it affect last point of a primitive or not ?)

class VPath : public VObject {
public:
    VPath();
    virtual ~VPath();
    
    virtual void draw( QPainter& painter );
    
    // postscript-like commands:
    void moveTo( const double& x, const double& y );
    void rmoveTo( const double& dx, const double& dy );    
    void lineTo( const double& x, const double& y );
    void rlineTo( const double& dx, const double& dy );
    void curveTo( const double& x1, const double& y1, const double& x2,
		    const double& y2, const double& x3, const double& y3 );
    void rcurveTo();    

    virtual void translate( const double& dx, const double& dy );

    void close();
    bool isClosed() { return( m_isClosed ); }


private:
    // a path consists of segments which own at least 2 points (lines).
    // bezier-curves have 2 additional control-points.
    struct Segment {
	// p0 is missing to avoid multiple vpoints
	VPoint* p1;
	VPoint* p2;
	VPoint* p3;	
    };
    bool m_isClosed;
    QList<Segment> m_segments;
};

#endif
