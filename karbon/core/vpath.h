#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>

#include "vobject.h"

class VPoint;

/**
 * VPaths are the most common high-level objects. They consist of lines and
 * beziers.
 */

// TODO: refine moveto-behaviour (should it affect last point of a primitive or not ?)

class VPath : public VObject {
public:
    VPath();
    virtual ~VPath();
    
    virtual void draw( QPainter& painter );
    
    const VPoint* currentPoint() const;
    
    // postscript-compliant commands:
    void moveTo( const double& x, const double& y );
    void rmoveTo( const double& dx, const double& dy );
    void lineTo( const double& x, const double& y );
    void rlineTo( const double& dx, const double& dy );
    void curveTo( const double& x1, const double& y1, const double& x2,
		    const double& y2, const double& x3, const double& y3 );
    void rcurveTo( const double& dx1, const double& dy1, const double& dx2,
		    const double& dy2, const double& dx3, const double& dy3 );
    void arcTo( const double& x1, const double& y1,
		    const double& x2, const double& y2, const double& r );

    virtual void translate( const double& dx, const double& dy );

    void close();
    bool isClosed() { return( m_isClosed ); }


private:
    // a segment is either a line or a bezier-curve:
    struct Segment {
	// p0 is omitted to avoid multiple vpoints
	VPoint* p1;	// control point
	VPoint* p2;	// control point
	VPoint* p3;	// end point	
    };
    bool m_isClosed;
    QList<Segment> m_segments;
};

#endif
