/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

#include "straight_connector.xpm"


static QPixmap *pIcon = NULL;

static KivioStencilSpawnerInfo sinfo = KivioStencilSpawnerInfo("Dave Marotti", "Straight Connector", "Basic Straight Connector", "0.1", "http://localhost/", "", "off");

extern "C" {
    KivioStencil *NewStencil();
    QPixmap *GetIcon();
    KivioStencilSpawnerInfo *GetSpawnerInfo();
}

KivioStencil *NewStencil()
{
    return new KivioStraightConnector();
}


//FIXME: Is this a memory leak?
QPixmap *GetIcon()
{
    if( pIcon )
        return pIcon;

    pIcon = new QPixmap( (const char **)straight_connector_xpm );
    return pIcon;
}

KivioStencilSpawnerInfo *GetSpawnerInfo()
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
}

KivioStraightConnector::~KivioStraightConnector()
{
   delete m_startAH;
   delete m_endAH;
    // FIXME: THe parents destructor gets called right?
}

void KivioStraightConnector::setStartPoint( float x, float y )
{
    m_pStart->setPosition( x, y, false );
    m_pStart->disconnect();
}

void KivioStraightConnector::setEndPoint( float x, float y )
{
    m_pEnd->setPosition( x, y, false );
    m_pEnd->disconnect();
}

KivioCollisionType KivioStraightConnector::checkForCollision( KivioPoint *p, float threshold )
{
    const float end_thresh = 4.0f;

    float px = p->x();
    float py = p->y();

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
		       threshold )==true )
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

    return pStencil;
}

void KivioStraightConnector::paint( KivioIntraStencilData *pData )
{
    KivioPainter *painter = pData->painter;
    float scale = pData->scale;
    float x1, y1, x2, y2;
    float vecX, vecY;
    float startCut, endCut;
    float len;


    painter->setFGColor( m_pLineStyle->color() );
    painter->setLineWidth( m_pLineStyle->width() * pData->scale );

    x1 = m_pStart->x() * scale;
    x2 = m_pEnd->x() * scale;

    y1 = m_pStart->y() * scale;
    y2 = m_pEnd->y() * scale;


    // Calculate the direction vector from start -> end
    vecX = m_pEnd->x() - m_pStart->x();
    vecY = m_pEnd->y() - m_pStart->y();

    // Normalize the vector
    len = sqrt( vecX*vecX + vecY*vecY );
    if( len )
    {
        vecX /= len;
        vecY /= len;

        // The amount we should hack off each end of the line
        startCut = m_startAH->cut() * scale;
        endCut = m_endAH->cut() * scale;

        // Move the endpoints by the cuts
        x1 += vecX * startCut;
        y1 += vecY * startCut;

        x2 -= vecX * endCut;
        y2 -= vecY * endCut;
    }


    // Draw the line
    painter->drawLine( x1, y1, x2, y2 );


    // Now draw the arrow heads
    if( len )
    {
        painter->setBGColor( m_pFillStyle->color() );

        m_startAH->paint( painter, m_pStart->x(), m_pStart->y(), -vecX, -vecY, scale );
        m_endAH->paint( painter, m_pEnd->x(), m_pEnd->y(), vecX, vecY, scale );
    }
}

void KivioStraightConnector::paintOutline( KivioIntraStencilData *pData )
{
    paint(pData);
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
