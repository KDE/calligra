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
#include "straight_connector.h"

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
#include "tkmath.h"

#include <qcolor.h>
#include <qpixmap.h>
#include <kdebug.h>
#include <KoZoomHandler.h>

#include "straight_connector.xpm"


static QPixmap *pIcon = NULL;

static KivioStencilSpawnerInfo sinfo = KivioStencilSpawnerInfo("Dave Marotti", "Straight Connector", "Dave Marotti - Straight Connector", "Basic Straight Connector", "0.1", "http://localhost/", "", "off");

#include <kgenericfactory.h>
K_EXPORT_COMPONENT_FACTORY( straight_connector, KGenericFactory<KivioConnectorFactory>( "KivioConnectorFactory" ) )

KivioConnectorFactory::KivioConnectorFactory(QObject *parent, const char* name, const QStringList& args) :
	KivioStencilFactory(parent, name, args)
{
	kdDebug(43000) << "new straight line connector factory: " << endl;
}

KivioStencil *KivioConnectorFactory::NewStencil()
{
    return new KivioStraightConnector();
}

KivioStencil *KivioConnectorFactory::NewStencil(const QString&)
{
    return new KivioStraightConnector();
}

//FIXME: Is this a memory leak?
QPixmap *KivioConnectorFactory::GetIcon()
{
    if( pIcon )
        return pIcon;

    pIcon = new QPixmap( (const char **)straight_connector_xpm );
    return pIcon;
}

KivioStencilSpawnerInfo *KivioConnectorFactory::GetSpawnerInfo()
{
    return &sinfo;
}


KivioStraightConnector::KivioStraightConnector()
   : Kivio1DStencil()
{
   m_pStart->setPosition(0.0f, 0.0f, false);
   m_pEnd->setPosition(72.0f, 72.0f, false);

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
   setType(kstConnector);
}

KivioStraightConnector::~KivioStraightConnector()
{
   delete m_startAH;
   delete m_endAH;
}

void KivioStraightConnector::setStartPoint( double x, double y )
{
    m_pStart->setPosition( x, y, false );
    m_pStart->disconnect();
}

void KivioStraightConnector::setEndPoint( double x, double y )
{
    m_pEnd->setPosition( x, y, false );
    m_pEnd->disconnect();

    if( m_needsText )
    {
       m_pTextConn->setPosition( (m_pStart->x()+m_pEnd->x())/2.0f,
        (m_pStart->y()+m_pEnd->y())/2.0f,
        false );
    }
}

KivioCollisionType KivioStraightConnector::checkForCollision( KoPoint *p, double threshold )
{
    const double end_thresh = 4.0f;

    double px = p->x();
    double py = p->y();

    KivioConnectorPoint *pPoint;

    int i = kctCustom + 1;
    pPoint = m_pConnectorPoints->first();
    while( pPoint )
    {
        if( px >= pPoint->x() - end_thresh &&
            px <= pPoint->x() + end_thresh &&
            py >= pPoint->y() - end_thresh &&
            py <= pPoint->y() + end_thresh )
        {
            return (KivioCollisionType)i;
        }

        i++;
        pPoint = m_pConnectorPoints->next();
    }


    if( collisionLine( m_pStart->x(), m_pStart->y(),
          m_pEnd->x(), m_pEnd->y(),
          px, py,
          threshold ) )
    {
      return kctBody;
    }

    return kctNone;
}

KivioStencil *KivioStraightConnector::duplicate()
{
    KivioStraightConnector *pStencil = new KivioStraightConnector();

    copyBasicInto( pStencil );

    // Copy the arrow head information
    pStencil->setStartAHType( m_startAH->type() );
    pStencil->setStartAHWidth( m_startAH->width() );
    pStencil->setStartAHLength( m_startAH->length() );

    pStencil->setEndAHType( m_endAH->type() );
    pStencil->setEndAHWidth( m_endAH->width() );
    pStencil->setEndAHLength( m_endAH->length() );

    *(pStencil->protection()) = *m_pProtection;
    *(pStencil->canProtect()) = *m_pCanProtect;

    return pStencil;
}

void KivioStraightConnector::paint( KivioIntraStencilData *pData )
{
  KivioPainter *painter = pData->painter;
  KoZoomHandler* zoomHandler = pData->zoomHandler;
  double x1, y1, x2, y2;
  double vecX, vecY;
  double len;


  painter->setLineStyle(m_pLineStyle);
  double lineWidth = m_pLineStyle->width();
  painter->setLineWidth(zoomHandler->zoomItY(lineWidth));

  x1 = zoomHandler->zoomItX(m_pStart->x());
  x2 = zoomHandler->zoomItX(m_pEnd->x());

  y1 = zoomHandler->zoomItY(m_pStart->y());
  y2 = zoomHandler->zoomItY(m_pEnd->y());


  // Calculate the direction vector from start -> end
  vecX = m_pEnd->x() - m_pStart->x();
  vecY = m_pEnd->y() - m_pStart->y();

  // Normalize the vector
  len = sqrt( vecX*vecX + vecY*vecY );
  if( len )
  {
    vecX /= len;
    vecY /= len;

    // Move the endpoints by the cuts
    x1 += vecX * zoomHandler->zoomItX(m_startAH->cut());
    y1 += vecY * zoomHandler->zoomItY(m_startAH->cut());

    x2 -= vecX * zoomHandler->zoomItX(m_endAH->cut());
    y2 -= vecY * zoomHandler->zoomItY(m_endAH->cut());
  }


  // Draw the line
  painter->drawLine( x1, y1, x2, y2 );


  // Now draw the arrow heads
  if( len )
  {
    painter->setBGColor( m_pFillStyle->color() );

    m_startAH->paint(painter, m_pStart->x(), m_pStart->y(), -vecX, -vecY, zoomHandler);
    m_endAH->paint(painter, m_pEnd->x(), m_pEnd->y(), vecX, vecY, zoomHandler);
  }

  // Text
  drawText(pData);
}

void KivioStraightConnector::paintOutline( KivioIntraStencilData *pData )
{
  KivioPainter *painter = pData->painter;
  KoZoomHandler* zoomHandler = pData->zoomHandler;
  double x1, y1, x2, y2;
  double vecX, vecY;
  double len;


  painter->setLineStyle(m_pLineStyle);
  double lineWidth = m_pLineStyle->width();
  painter->setLineWidth(zoomHandler->zoomItY(lineWidth));

  x1 = zoomHandler->zoomItX(m_pStart->x());
  x2 = zoomHandler->zoomItX(m_pEnd->x());

  y1 = zoomHandler->zoomItY(m_pStart->y());
  y2 = zoomHandler->zoomItY(m_pEnd->y());


  // Calculate the direction vector from start -> end
  vecX = m_pEnd->x() - m_pStart->x();
  vecY = m_pEnd->y() - m_pStart->y();

  // Normalize the vector
  len = sqrt( vecX*vecX + vecY*vecY );
  if( len )
  {
    vecX /= len;
    vecY /= len;

    // Move the endpoints by the cuts
    x1 += vecX * zoomHandler->zoomItX(m_startAH->cut());
    y1 += vecY * zoomHandler->zoomItY(m_startAH->cut());

    x2 -= vecX * zoomHandler->zoomItX(m_endAH->cut());
    y2 -= vecY * zoomHandler->zoomItY(m_endAH->cut());
  }


  // Draw the line
  painter->drawLine( x1, y1, x2, y2 );


  // Now draw the arrow heads
  if( len )
  {
    painter->setBGColor( m_pFillStyle->color() );

    m_startAH->paint(painter, m_pStart->x(), m_pStart->y(), -vecX, -vecY, zoomHandler);
    m_endAH->paint(painter, m_pEnd->x(), m_pEnd->y(), vecX, vecY, zoomHandler);
  }

  // Text
  // Don't paint text in outline mode as it makes moving harder
  drawText(pData);
}

bool KivioStraightConnector::saveCustom( QDomElement &e, QDomDocument &doc )
{
   e.appendChild( saveArrowHeads(doc) );

   return true;
}

bool KivioStraightConnector::loadCustom( const QDomElement &e )
{
   QDomNode node;
   QString name;

   node = e.firstChild();
   while( !node.isNull() )
   {
      name = node.nodeName();
      if( name == "KivioArrowHeads" )
      {
	 loadArrowHeads( node.toElement() );
      }

      node = node.nextSibling();
   }

   updateGeometry();

   return true;
}

QDomElement KivioStraightConnector::saveArrowHeads( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioArrowHeads");

    e.appendChild( m_startAH->saveXML(doc) );
    e.appendChild( m_endAH->saveXML(doc) );

    return e;
}

bool KivioStraightConnector::loadArrowHeads( const QDomElement &e )
{
    QDomNode node;
    QString nodeName;
    QDomElement arrowE;
    bool first=true;

    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        arrowE = node.toElement();

        if( nodeName == "KivioArrowHead" )
        {
            if( first==true )
            {
                m_startAH->loadXML(arrowE);

                first = false;
            }
            else
            {
                m_endAH->loadXML(arrowE);
            }
        }

        node = node.nextSibling();
    }

    return true;
}

bool KivioStraightConnector::hasTextBox() const
{
  return true;
}

#include "straight_connector.moc"
