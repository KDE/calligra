/*
* Kivio - Visual Modelling and Flowcharting
* Copyright (C) 2000-2001 theKompany.com & Dave Marotti
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <stdio.h>
#include <math.h>
#include "sml_connector.h"

#include "kivio_common.h"
#include "kivio_connector_target.h"
#include "kivio_layer.h"
#include "kivio_line_style.h"
#include "kivio_page.h"
#include "kivio_painter.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_text_style.h"
#include "kivio_shape.h"
#include "kivio_shape_data.h"
#include "kivio_point.h"

#include "tkmath.h"

#include <QColor>
#include <qpixmap.h>
#include <kdebug.h>
#include <KoZoomHandler.h>
#include <kiconloader.h>

static KivioStencilSpawnerInfo sinfo = KivioStencilSpawnerInfo( "Ian Reinhart Geiser", "SML Connector", "SML Connector", "SML Based Connector", "0.1", "http://localhost/", "", "off" );
#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( sml_connector, KGenericFactory<KivioSMLConnectorFactory>( "KivioSMLConnectorFactory" ) )

KivioSMLConnectorFactory::KivioSMLConnectorFactory( QObject *parent, const char* name, const QStringList& args ) :
		KivioStencilFactory( parent, name, args )
{
	kDebug(43000) << "new sml stencil factory: " << endl;
}

KivioStencil *KivioSMLConnectorFactory::NewStencil( const QString& name )
{
	return new KivioSMLConnector( name );
}

KivioStencil *KivioSMLConnectorFactory::NewStencil()
{
	return new KivioSMLConnector( "basic_line" );
}


QPixmap *KivioSMLConnectorFactory::GetIcon()
{
	return & BarIcon( "SML_conn" );
}

KivioStencilSpawnerInfo *KivioSMLConnectorFactory::GetSpawnerInfo()
{
	return & sinfo;
}

KivioSMLConnector::KivioSMLConnector( const QString &name )
		: Kivio1DStencil(), m_name( name )
{

	m_pStart->setPosition( 0.0f, 0.0f, false );
	m_pEnd->setPosition( 72.0f, 72.0f, false );

	m_startAH = new KivioArrowHead();
	m_endAH = new KivioArrowHead();
	m_needsWidth = false;
	m_needsText = true;

	m_pCanProtect->clearBit( kpAspect );
	m_pCanProtect->clearBit( kpWidth );
	m_pCanProtect->clearBit( kpHeight );
	m_pCanProtect->clearBit( kpX );
	m_pCanProtect->clearBit( kpY );

	// This is a stencil of type connector
	setType( kstConnector );

	// We only have one shape, an open path.
	//m_shape = new KivioShape();
	//m_shape->shapeData() ->setShapeType( KivioShapeData::kstOpenPath );
	loadPath( name );
}

KivioSMLConnector::~KivioSMLConnector()
{
	delete m_startAH;
	delete m_endAH;
}

void KivioSMLConnector::setStartPoint( double x, double y )
{
	m_pStart->setPosition( x, y, false );
	m_pStart->disconnect();

	if ( m_needsText )
	{
		m_pTextConn->setPosition( ( m_pEnd->x() + m_pStart->x() ) / 2.0f,
		                          ( m_pEnd->y() + m_pStart->y() ) / 2.0f,
		                          false );
	}

	// Set list end point
	m_pConnectorPoints ->first() ->setX( x );
	m_pConnectorPoints ->first() ->setY( y );

}

void KivioSMLConnector::setEndPoint( double x, double y )
{
	m_pEnd->setPosition( x, y, false );
	m_pEnd->disconnect();
	m_pConnectorPoints ->last() ->setX( x );
	m_pConnectorPoints ->last() ->setY( y );
}

KivioCollisionType KivioSMLConnector::checkForCollision( KivioPoint *p, double threshold )
{
	const double end_thresh = 4.0f;

	double px = p->x();
	double py = p->y();

	KivioConnectorPoint *pPoint;

	int i = kctCustom + 1;
	pPoint = m_pConnectorPoints->first();
	while ( pPoint )
	{
		if ( px >= pPoint->x() - end_thresh &&
		        px <= pPoint->x() + end_thresh &&
		        py >= pPoint->y() - end_thresh &&
		        py <= pPoint->y() + end_thresh )
		{
			return ( KivioCollisionType ) i;
		}

		i++;
		pPoint = m_pConnectorPoints->next();
	}


	if ( collisionLine( m_pStart->x(), m_pStart->y(),
	                    m_pEnd->x(), m_pEnd->y(),
	                    px, py,
	                    threshold ) )
	{
		return kctBody;
	}

	return kctNone;
}

KivioStencil *KivioSMLConnector::duplicate()
{
	KivioSMLConnector * pStencil = new KivioSMLConnector( m_name );


	// TODO: Copy internal pointlist information?

	// Copy the arrow head information
	pStencil->setStartAHType( m_startAH->type() );
	pStencil->setStartAHWidth( m_startAH->width() );
	pStencil->setStartAHLength( m_startAH->length() );

	pStencil->setEndAHType( m_endAH->type() );
	pStencil->setEndAHWidth( m_endAH->width() );
	pStencil->setEndAHLength( m_endAH->length() );

	*( pStencil->protection() ) = *m_pProtection;
	*( pStencil->canProtect() ) = *m_pCanProtect;

	return pStencil;
}
/*
void KivioSMLConnector::drawOpenPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
	KivioPainter * painter;
	KivioShapeData *pShapeData = pShape->shapeData();
	KivioPoint *pPoint, *pNewPoint;
	QPtrList <KivioPoint> *pPointList = pShapeData->pointList();

	QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
	pNewPoints->setAutoDelete( true );

	pPoint = pPointList->first();
	while ( pPoint )
	{
		pNewPoint = new KivioPoint( pData->zoomHandler->zoomItX( pPoint->x() ),
		                            pData->zoomHandler->zoomItY( pPoint->y() ),
		                            pPoint->pointType() );
		pNewPoints->append( pNewPoint );

		pPoint = pPointList->next();
	}

	painter = pData->painter;
	double lineWidth = pShapeData->lineStyle() ->width();
	painter->setLineWidth( pData->zoomHandler->zoomItY( lineWidth ) );
	painter->setFGColor( pShapeData->lineStyle() ->color() );

	painter->drawOpenPath( pNewPoints );
	delete pNewPoints;
}
*/
void KivioSMLConnector::paint( KivioIntraStencilData *pData )
{
	KivioPainter * painter = pData->painter;
	KoZoomHandler* zoomHandler = pData->zoomHandler;
	double x1, y1, x2, y2;
	double vecX, vecY;
	double len;


	painter->setFGColor( m_pLineStyle->color() );
	painter->setLineWidth( zoomHandler->zoomItY( m_pLineStyle->width() ) );

	x1 = zoomHandler->zoomItX( m_pStart->x() );
	x2 = zoomHandler->zoomItX( m_pEnd->x() );

	y1 = zoomHandler->zoomItY( m_pStart->y() );
	y2 = zoomHandler->zoomItY( m_pEnd->y() );


	// Calculate the direction vector from start -> end
	vecX = m_pEnd->x() - m_pStart->x();
	vecY = m_pEnd->y() - m_pStart->y();

	// Normalize the vector
	len = sqrt( vecX * vecX + vecY * vecY );
	if ( len )
	{
		vecX /= len;
		vecY /= len;

		// Move the endpoints by the cuts
		x1 += vecX * zoomHandler->zoomItX( m_startAH->cut() );
		y1 += vecY * zoomHandler->zoomItY( m_startAH->cut() );

		x2 -= vecX * zoomHandler->zoomItX( m_endAH->cut() );
		y2 -= vecY * zoomHandler->zoomItY( m_endAH->cut() );
	}


	// Build shape up from m_pConnectorPoints


	QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
	pNewPoints->setAutoDelete( true );

	pNewPoints->append(new KivioPoint( zoomHandler->zoomItX( m_pStart->x() ),
		                            zoomHandler->zoomItY( m_pStart->y() )));

	KivioConnectorPoint *pPoint = m_PointList.first();
	while ( pPoint )
	{
		KivioPoint *pNewPoint = new KivioPoint( zoomHandler->zoomItX( pPoint->x() ),
		                            zoomHandler->zoomItY( pPoint->y() ));
		pNewPoints->append( pNewPoint );

		pPoint = m_PointList.next();
	}

	pNewPoints->append(new KivioPoint( zoomHandler->zoomItX( m_pEnd->x() ),
		                            zoomHandler->zoomItY( m_pEnd->y() )));
	painter = pData->painter;
	double lineWidth = m_pLineStyle ->width();
	painter->setLineWidth( zoomHandler->zoomItY( lineWidth ) );
	painter->setFGColor( m_pLineStyle ->color() );

	painter->drawOpenPath( pNewPoints );
	delete pNewPoints;

	// Now draw the arrow heads
	if ( len )
	{
		painter->setBGColor( m_pFillStyle->color() );

		m_startAH->paint( painter, m_pStart->x(), m_pStart->y(), -vecX, -vecY, zoomHandler );
		m_endAH->paint( painter, m_pEnd->x(), m_pEnd->y(), vecX, vecY, zoomHandler );
	}

	// TODO: Text
	drawText( pData );
}

void KivioSMLConnector::paintOutline( KivioIntraStencilData *pData )
{
	paint( pData );
}

bool KivioSMLConnector::saveCustom( QDomElement &e, QDomDocument &doc )
{
	kDebug(43000) << "Save custom " << endl;
	e.appendChild( saveArrowHeads( doc ) );

	//QDomElement type = m_shape->saveXML(doc);
	KivioConnectorPoint *p = m_PointList.first();
	while ( p )
	{
			QDomElement pt = p->saveXML( doc );
			e.appendChild( pt );
			p = m_PointList.next();
	}
	return true;
}

bool KivioSMLConnector::loadCustom( const QDomElement &e )
{
	kDebug(43000) << "Load custom " << endl;
	QDomNode node;
	QString name;

	node = e.firstChild();
	while ( !node.isNull() )
	{
		name = node.nodeName();
		if ( name == "KivioArrowHeads" )
		{
			loadArrowHeads( node.toElement() );
		}
		else if ( name == "KivioShape" )
		{
			//m_shape->loadXML(node.toElement());
			QDomNode pts = node.firstChild();
			while ( !pts.isNull() )
			{
				KivioConnectorPoint * pt = new KivioConnectorPoint();
				pt->loadXML( pts.toElement() );
				m_pConnectorPoints->append( pt );
				m_PointList.append( pt );
				pts = pts.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	updateGeometry();

	return true;
}

QDomElement KivioSMLConnector::saveArrowHeads( QDomDocument &doc )
{
	QDomElement e = doc.createElement( "KivioArrowHeads" );

	e.appendChild( m_startAH->saveXML( doc ) );
	e.appendChild( m_endAH->saveXML( doc ) );

	return e;
}

bool KivioSMLConnector::loadArrowHeads( const QDomElement &e )
{
	QDomNode node;
	QString nodeName;
	QDomElement arrowE;
	bool first = true;

	node = e.firstChild();
	while ( !node.isNull() )
	{
		nodeName = node.nodeName();
		arrowE = node.toElement();

		if ( nodeName == "KivioArrowHead" )
		{
			if ( first == true )
			{
				m_startAH->loadXML( arrowE );

				first = false;
			}
			else
			{
				m_endAH->loadXML( arrowE );
			}
		}

		node = node.nextSibling();
	}

	return true;
}


bool KivioSMLConnector::loadPath( const QString &file )
{
	kDebug(43000) << "Loading :" << file << endl;

	m_PointList.clear();
	m_PointList.append(m_pStart);

	KivioConnectorPoint *pt = new KivioConnectorPoint(this,false);
	pt->setPosition(m_pStart->x()+10,m_pStart->y()+10,false);
//	m_PointList.append( pt );
	m_pConnectorPoints->append(pt);

	pt = new KivioConnectorPoint(this,false);
	pt->setPosition(m_pStart->x()+20,m_pStart->y()+20,false);
	m_PointList.append( pt );
	m_pConnectorPoints->append(pt);

	pt = new KivioConnectorPoint(this,false);
	pt->setPosition(m_pStart->x()+30,m_pStart->y()+30,false);
	m_PointList.append( pt );
	m_pConnectorPoints->append(pt);

//	m_PointList.append(m_pEnd);
	m_name = file;
        return true;
}

#include "sml_connector.moc"
