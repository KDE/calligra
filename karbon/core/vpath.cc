#include <kdebug.h>

#include <qpainter.h>
#include <qpointarray.h>
#include <math.h>

#include "vpath.h"
#include "vpoint.h"

VPath::VPath()
    : VObject(), m_isClosed(false)
{
    // we always need a current point (0.0,0.0):
    m_segments.append( new Segment );
    m_segments.getLast()->p1 = 0L;
    m_segments.getLast()->p2 = 0L;
    m_segments.getLast()->p3 = new VPoint();
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
    VPoint* prevPoint(0L);	// previous point (for calculating beziers)    
    QPointArray qpa;
    
    if ( segment!=0L )
    {
	prevPoint = segment->p3;
	
	if ( !isClosed() ) // only paint first point if path isnt closed:
	{
	    qpa.resize( qpa.size()+1 );
	    qpa.setPoint( qpa.size()-1, segment->p3->getQPoint() );
	}
    }

    for ( segment=m_segments.next(); segment!=0L; segment=m_segments.next() ) 
    {
    	// draw bezier-curve if all points are available:
	if ( prevPoint!=0L && segment->p1!=0L && segment->p2!=0L && segment->p3!=0L )
	{
	    // let qt calculate bezier-qpoints for us:
	    QPointArray pa(4);
	    pa.setPoint( 0, prevPoint->getQPoint() );
	    pa.setPoint( 1, segment->p1->getQPoint() );
	    pa.setPoint( 2, segment->p2->getQPoint() );
	    pa.setPoint( 3, segment->p3->getQPoint() );
	    pa = pa.quadBezier(); // is this a memory leak ?

	    // can this part be made more efficient ? i bet it could...
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

const VPoint*
VPath::currentPoint() const 
{
    return( m_segments.getLast()->p3 );
}

void
VPath::moveTo( const double& x, const double& y )
{
    if ( isClosed() ) return;

    m_segments.getLast()->p3->moveTo( x, y );
}

void
VPath::rmoveTo( const double& dx, const double& dy )
{
    if ( isClosed() ) return;

    m_segments.getLast()->p3->rmoveTo( dx, dy );
}

void
VPath::lineTo( const double& x, const double& y )
{
    if ( isClosed() ) return;

    m_segments.append( new Segment );
    m_segments.getLast()->p1 = 0L;
    m_segments.getLast()->p2 = 0L;
    m_segments.getLast()->p3 = new VPoint( x, y );
}

void
VPath::curveTo( const double& x1, const double& y1, const double& x2,
    const double& y2, const double& x3, const double& y3 )
{
    if ( isClosed() ) return;

    m_segments.append( new Segment );
    m_segments.getLast()->p1 = new VPoint( x1, y1 );
    m_segments.getLast()->p2 = new VPoint( x2, y2 );
    m_segments.getLast()->p3 = new VPoint( x3, y3 );
}

void
VPath::arcTo( const double& x1, const double& y1,
    const double& x2, const double& y2, const double& r )
{
    if ( isClosed() ) return;

    // we need to calculate the tangent points. therefore calculate tangents
    // D10=P1P0 and D12=P1P2 first:
    double dx10 = m_segments.getLast()->p3->x() - x1;
    double dy10 = m_segments.getLast()->p3->y() - y1;
    double dx12 = x2 - x1;
    double dy12 = y2 - y1;

    // calculate distance squares:
    double dsq10 = dx10*dx10 + dy10*dy10;
    double dsq12 = dx12*dx12 + dy12*dy12;

    // we now calculate tan(a/2) where a is the angular between D10 and D12.
    // we take advantage of D10*D12=d10*d12*cos(a), |D10xD12|=d10*d12*sin(a)
    // and tan(a/2)=sin(a)/[1-cos(a)].
    double num   = dx10*dy12 - dy10*dx12;
    double denom = sqrt(dsq10*dsq12) - dx10*dx12 + dy10*dy12;

    if ( denom==0 )	// points are co-linear
	lineTo( x1, y1 );	// just add a line to first point
    else
    {
        // calculate distances from P1 to tangent points:
        double dist = fabs(r*num/denom);
	double d1t0 = dist/sqrt(dsq10);
	double d1t1 = dist/sqrt(dsq12);

// TODO: check for r<0
	
	double bx0 = x1+dx10*d1t0;
	double by0 = y1+dy10*d1t0;

	// if (bx0,by0) deviates from current point, add a line to it:
	// TODO: decide via radius<XXX or sthg?
	if ( bx0 != m_segments.getLast()->p3->x() || by0 != m_segments.getLast()->p3->y() )
	    lineTo( bx0, by0 );
	    	
	double bx3 = x1+dx12*d1t1;
	double by3 = y1+dy12*d1t1;

	// the two bezier-control points are located on the tangents at a fraction
	// of the distance [tangent points<->tangent intersection].
	double distsq = (x1 - bx0)*(x1 - bx0) + (y1 - by0)*(y1 - by0);
	double rsq = r*r;
	double fract;

// TODO: make this nicer?

	if ( distsq >= rsq * VPoint::s_fractScale ) // r is very small
    	    fract = 0.0; // dist==r==0
	else
    	    fract = (4.0 / 3.0) / (1 + sqrt(1 + distsq / rsq));

	double bx1 = bx0 + (x1 - bx0) * fract;
	double by1 = by0 + (y1 - by0) * fract;
	double bx2 = bx3 + (x1 - bx3) * fract;
	double by2 = by3 + (y1 - by3) * fract;
	
	// finally add the bezier-segment:
	m_segments.append( new Segment );
        m_segments.getLast()->p1 = new VPoint( bx1, by1 );
	m_segments.getLast()->p2 = new VPoint( bx2, by2 );
	m_segments.getLast()->p3 = new VPoint( bx3, by3 );
    }
}

void
VPath::translate( const double& dx, const double& dy )
{
    Segment* segment = m_segments.first();	// first segment

    // only translate first point if path isnt closed:
    if ( !isClosed() )
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
