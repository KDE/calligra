#include <kdebug.h>

#include <qpoint.h>
#include <qpointarray.h>

#include "vpoint.h"
#include "vprimitive.h"

void
VLine::getQPoints( QPointArray& qpa )
{
    if ( m_firstPoint && m_lastPoint )	 // we need all points
    {
	qpa.resize( qpa.size()+2 );
	qpa.setPoint( qpa.size()-2, m_firstPoint->getQPoint() );
	qpa.setPoint( qpa.size()-1, m_lastPoint->getQPoint() );
    }
}

void
VBezier::getQPoints( QPointArray& qpa )
{
    if ( m_firstPoint && m_lastPoint && m_firstCtrlPoint && m_lastCtrlPoint ) // we need all points
    {
	QPointArray pa(4);
	pa.setPoint( 0, m_firstPoint->getQPoint() );
	pa.setPoint( 1, m_firstCtrlPoint->getQPoint() );
	pa.setPoint( 2, m_lastCtrlPoint->getQPoint() );
	pa.setPoint( 3, m_lastPoint->getQPoint() );
	pa = pa.quadBezier(); // is this a memory leak ?
	
	// can this be made more effective ? i bet it can...
	unsigned int size1( qpa.size() ), size2( pa.size() );
	qpa.resize( size1+size2 );
	for ( unsigned int i=0; i<size2; i++ )
	{
	    qpa.setPoint( size1+i, pa.point(i) );
	}
    }
}
