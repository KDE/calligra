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

    m_points.append( new VPoint() );	// we always need a current point (0.0,0.0)
}

VPath::~VPath()
{
    for ( VPrimitive* primitive=m_primitives.first(); primitive!=0L; primitive=m_primitives.next() ) 
    {
	delete( primitive );
    }    
    for ( VPoint* point=m_points.first(); point!=0L; point=m_points.next() ) 
    {
	delete( point );
    }
}

void
VPath::moveTo( double& x, double& y )
{
    m_points.getLast()->moveTo( x, y );
}

void
VPath::rmoveTo( double& x, double& y )
{
    m_points.getLast()->rmoveTo( x, y );
}

void
VPath::lineTo( double& x, double& y )
{
    VPoint* newpoint = new VPoint( x, y );
    m_primitives.append( new VLine( m_points.getLast(), newpoint ) );
    m_points.append( newpoint );
}

void
VPath::curveTo( double& x1, double& y1, double& x2, double& y2, double& x3, double& y3 )
{
    VPoint* newpoint1 = new VPoint( x1, y1 );
    VPoint* newpoint2 = new VPoint( x2, y2 );
    VPoint* newpoint3 = new VPoint( x3, y3 );
    m_primitives.append( new VBezier( m_points.getLast(), newpoint1, newpoint2, newpoint3 ) );
    m_points.append( newpoint1 );
    m_points.append( newpoint2 );
    m_points.append( newpoint3 );
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
    painter.setPen( Qt::black );
    painter.drawPolyline( qpa );
    painter.restore();
}
