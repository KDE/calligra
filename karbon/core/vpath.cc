#include <kdebug.h>

#include <qpainter.h>
#include <qpointarray.h>

#include "vpath.h"
#include "vpoint.h"

// TODO: think about closing path: should last segment use vpoint of first segment ?

VPath::VPath()
    : VObject(), m_isClosed(false)
{
    m_segments.append( new Segment );
    m_segments.getLast()->p1 = 0L;
    m_segments.getLast()->p2 = 0L;
    m_segments.getLast()->p3 = new VPoint(); // we always need a current point (0.0,0.0)
}

VPath::~VPath()
{
    for ( Segment* segment=m_segments.first(); segment!=0L; segment=m_segments.next() ) 
    {
	if ( segment->p1 )	// control-points are never shared
	    delete( segment->p1 );
	if ( segment->p2 )
	    delete( segment->p2 );
	if ( segment->p3 && segment->p3->unref()==0 )
	    delete( segment->p3 );
    }
}

void
VPath::draw( QPainter& painter )
{
    // (walk down all Segments and add their QPoints into a QPointArray)

    Segment* segment = m_segments.first();	// first segment
    VPoint* prevPoint(0L);	// previous point (for beziers)    
    QPointArray qpa;
    
    // only paint first point if path isnt closed:
    if ( segment!=0L && !isClosed() )
    {
	qpa.resize( qpa.size()+1 );
	qpa.setPoint( qpa.size()-1, segment->p3->getQPoint() );
	prevPoint = segment->p3;
    }

    for ( segment=m_segments.next(); segment!=0L; segment=m_segments.next() ) 
    {
    	// draw bezier-curve if all points are available:
	if ( prevPoint!=0L && segment->p1 && segment->p2 && segment->p3 )
	{
	    // let qt calculate us bezier-qpoints:
	    QPointArray pa(4);
	    pa.setPoint( 0, prevPoint->getQPoint() );
	    pa.setPoint( 1, segment->p1->getQPoint() );
	    pa.setPoint( 2, segment->p2->getQPoint() );
	    pa.setPoint( 3, segment->p3->getQPoint() );
	    pa = pa.quadBezier(); // is this a memory leak ?

	    // can this part be made more eficient ? i bet it can...
	    unsigned int size1(qpa.size()), size2(pa.size());
	    qpa.resize( size1+size2 );
	    for ( unsigned int i=0; i<size2; i++ )
	    {
		qpa.setPoint( size1+i, pa.point(i) );
	    }
	}
	else
	{
	    // draw a line:
	    qpa.resize( qpa.size()+1 );
	    qpa.setPoint( qpa.size()-1, segment->p3->getQPoint() );
	}
	prevPoint = segment->p3; // need previous point to calculate bezier-qpoints
    }
    
    painter.save();
    painter.setPen( Qt::black );
    painter.setBrush( Qt::blue );

    // draw open or closed path ?
    if ( isClosed() )
	painter.drawPolygon( qpa );
    else
	painter.drawPolyline( qpa );
    painter.restore();
}

void
VPath::moveTo( const double& x, const double& y )
{
    m_segments.getLast()->p3->moveTo( x, y );
}

void
VPath::rmoveTo( const double& dx, const double& dy )
{
    m_segments.getLast()->p3->rmoveTo( dx, dy );
}

void
VPath::lineTo( const double& x, const double& y )
{
    if ( !isClosed() )
    {
	m_segments.append( new Segment );
        m_segments.getLast()->p1 = 0L;
        m_segments.getLast()->p2 = 0L;
        m_segments.getLast()->p3 = new VPoint( x, y );
    }
}

void
VPath::curveTo( const double& x1, const double& y1, const double& x2,
    const double& y2, const double& x3, const double& y3 )
{
    if ( !isClosed() )
    {
        m_segments.append( new Segment );
        m_segments.getLast()->p1 = new VPoint( x1, y1 );
        m_segments.getLast()->p2 = new VPoint( x2, y2 );
        m_segments.getLast()->p3 = new VPoint( x3, y3 );
    }
}

void
VPath::translate( const double& dx, const double& dy )
{
    Segment* segment = m_segments.first();	// first segment

    // only translate first point if path isnt closed:
    if ( segment!=0L && !isClosed() )
    {
	if ( segment->p3 )
	    segment->p3->rmoveTo( dx, dy );
    }
    
    for ( segment=m_segments.next(); segment!=0L; segment=m_segments.next() ) 
    {
	if ( segment->p1 )
	    segment->p1->rmoveTo( dx, dy );
	if ( segment->p2 )
	    segment->p2->rmoveTo( dx, dy );
	if ( segment->p3 )
	    segment->p3->rmoveTo( dx, dy );
    }
}

void
VPath::close()
{
    // TODO: dont "close" a single line
    // draw a line if last point differs from first point
    if ( *(m_segments.getFirst()->p3) != *(m_segments.getLast()->p3) )
    {
	lineTo( m_segments.getFirst()->p3->x(), m_segments.getFirst()->p3->y() );
    }
    
    // do nothing if first and last point are the same (eg only first point exists):
    if ( m_segments.getFirst()->p3!=m_segments.getLast()->p3 )
    {
	if ( m_segments.getLast()->p3->unref()==0 )
	    delete m_segments.getLast()->p3;
	    
	m_segments.getLast()->p3 = m_segments.getFirst()->p3;
	
	m_isClosed = true;
    }
}
