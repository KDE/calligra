#include <qpainter.h>
#include <qpointarray.h>

#include "vpath.h"
#include "vpoint.h"

VPath::VPath()
    : VObject()
{
    // let's see if using autodelete brings us into hell:
    m_primitives.setAutoDelete(true);
    m_points.setAutoDelete(true);

    m_currentPoint = new VPoint();	// we always need a current point (0.0,0.0)
}

VPath::~VPath()
{
    // we use autodelete to destroy our points and primitives
    
// TODO: refcounting, what about copy-ctor ?
    if ( m_currentPoint )
	delete m_currentPoint;
}

void
VPath::moveTo( double& x, double& y )
{
    m_currentPoint->moveTo( x, y );
}

void
VPath::lineTo( double& x, double& y )
{
// TODO: convention: does moveTo change last primitve ?
    VPoint* newpoint = new VPoint( x, y );
    m_points.append( m_currentPoint );
    m_primitives.append( new VLine( m_currentPoint, newpoint ) );
    m_currentPoint = newpoint;
}

void
VPath::draw( QPainter& painter )
{
    QPointArray qpa;
    // walk down all VPrimitives and store their QPoints into a QPointArray:
    for ( VPrimitive* primitive=m_primitives.first(); primitive!=0L; primitive=m_primitives.next() ) 
    {
	primitive->getQPoints( qpa );	// note: vbeziers give more than 2 qpoints
    }
    
    painter.save();
    
    painter.restore();
}
