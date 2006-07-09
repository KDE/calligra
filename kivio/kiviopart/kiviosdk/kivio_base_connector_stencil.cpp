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

/*
 * Implementing a connector involves deriving from this class.
 *
 * You need to implement the following routines:
 *   setStartPoint
 *   setEndPoint
 *   checkForCollision
 *   duplicate
 *   paint
 *   paintOutline
 *   saveXML
 *   loadXML
 *
 * Save and Load should call saveConnectors, loadConnectors, saveProperties, and
 * loadProperties.  These are helper functions which take care of the common
 * acts of saving/loading connectors and colors/line-styles, etc...
 *
 */

#include "kivio_arrowhead.h"
#include "kivio_base_connector_stencil.h"
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_custom_drag_data.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_layer.h"
#include "kivio_line_style.h"
#include "kivio_page.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_stencil.h"

#include <kdebug.h>
#include <KoZoomHandler.h>

/**
 * Default constructor.
 *
 * This will allocate a new fill style object, a list for
 * connection points, and set some default values.
 */
KivioBaseConnectorStencil::KivioBaseConnectorStencil()
    : KivioStencil()
{
    m_pFillStyle = new KivioFillStyle();
    m_pLineStyle = new KivioLineStyle();

    m_pConnectorPoints = new QPtrList<KivioConnectorPoint>;
    m_pConnectorPoints->setAutoDelete(true);

}


/**
 * Destructor
 */
KivioBaseConnectorStencil::~KivioBaseConnectorStencil()
{
    delete m_pFillStyle;
    delete m_pLineStyle;
    delete m_pConnectorPoints;
}

void KivioBaseConnectorStencil::setFGColor( QColor c )
{
    m_pLineStyle->setColor(c);
}

QColor KivioBaseConnectorStencil::fgColor()
{
    return m_pLineStyle->color();
}

void KivioBaseConnectorStencil::setLineWidth( double f )
{
    m_pLineStyle->setWidth(f);
}

double KivioBaseConnectorStencil::lineWidth()
{
    return m_pLineStyle->width();
}

void KivioBaseConnectorStencil::setBGColor( QColor c )
{
    m_pFillStyle->setColor(c);
}

QColor KivioBaseConnectorStencil::bgColor()
{
    return m_pFillStyle->color();
}


/////////////////////////////////
// Position functions
/////////////////////////////////
void KivioBaseConnectorStencil::setX( double x )
{
    double dx = x - m_x;

    m_x += dx;

    KivioConnectorPoint *p = m_pConnectorPoints->first();
    while( p )
    {
        p->setX( p->x() + dx, false );
        p->disconnect();

        p = m_pConnectorPoints->next();
    }


    m_x = x;
}

void KivioBaseConnectorStencil::setY( double y )
{
    double dy = y - m_y;

    m_y += dy;

    KivioConnectorPoint *p = m_pConnectorPoints->first();
    while( p )
    {
        p->setY( p->y() + dy, false );
        p->disconnect();

        p = m_pConnectorPoints->next();
    }

    m_y = y;
}

void KivioBaseConnectorStencil::setPosition( double x, double y )
{
    double dx = x - m_x;
    double dy = y - m_y;

    m_x += dx;
    m_y += dy;

    KivioConnectorPoint *p = m_pConnectorPoints->first();
    while( p )
    {
        p->setPosition( p->x()+dx, p->y()+dy, false );
        p->disconnect();

        p = m_pConnectorPoints->next();
    }


    m_x = x;
    m_y = y;
}



/////////////////////////////
// Connection tool functions
/////////////////////////////
void KivioBaseConnectorStencil::setStartPoint( double /*x*/, double /*y*/ )
{
    /* Derived class must implement this function */
    // m_start.setPosition( x, y, false );
    // m_start.disconnect();
}


void KivioBaseConnectorStencil::setEndPoint( double /*x*/, double /*y*/ )
{
    /* Derived class must implement this function */
    // m_end.setPosition( x, y, false );
    // m_end.disconnect();
}


void KivioBaseConnectorStencil::paint( KivioIntraStencilData * )
{
    /* Derived class must implement this */
}

void KivioBaseConnectorStencil::paintOutline( KivioIntraStencilData *pData )
{
    /* Derived class should implement this */
    paint( pData );
}

void KivioBaseConnectorStencil::paintConnectorTargets( KivioIntraStencilData * )
{
}

void KivioBaseConnectorStencil::paintSelectionHandles( KivioIntraStencilData *pData )
{
  // Handle Width
  const double HW = 6.0f;
  const double HWP1 = HW+1.0f;

  // Handle Width Over 2
  const double HWo2 = HW/2.0f;

  // Stencil data
  KoZoomHandler* zoomHandler = pData->zoomHandler;
  KivioPainter *painter = pData->painter;
  double x1, y1;

  painter->setLineWidth(1.0f);
  painter->setFGColor(QColor(0,0,0));

  KivioConnectorPoint *p = m_pConnectorPoints->first();
  while( p )
  {
    x1 = zoomHandler->zoomItX(p->x()) - HWo2;
    y1 = zoomHandler->zoomItY(p->y()) - HWo2;

    if( p->target() )
        painter->setBGColor(QColor(200,0,0));
    else
        painter->setBGColor(QColor(0,200,0));

    painter->fillRect( x1, y1, HWP1, HWP1 );

    p = m_pConnectorPoints->next();
  }
}



///////////////////////////////
// Collision detection
///////////////////////////////
KivioCollisionType KivioBaseConnectorStencil::checkForCollision( KoPoint *, double )
{
    /* Derived class must implement this */
    return kctNone;
}




/////////////////////////////////
// Custom dragging
/////////////////////////////////
/**
 * Custom drag the connector points.
 *
 * The default action of this function is to locate the point
 * in the connector list by the id and then drag it around.
 * Then attempt to snap it to another stencil.  Otherwise
 * disconnect it.
 */
void KivioBaseConnectorStencil::customDrag( KivioCustomDragData *pData )
{
    double _x = pData->x;
    double _y = pData->y;
    int id = pData->id;

    KivioConnectorPoint *p;

    // Locate the point specified by id
    p = m_pConnectorPoints->at( id - (kctCustom+1));

    if( !p )
    {
       kdDebug(43000) << "KivioBaseConnectorStencil::customDrag() - KivioConnectorPoint id: " << (id - (kctCustom+1)) << "  not found\n" << endl;
       return;
    }

    p->setPosition( _x, _y, true );


    KivioLayer *pCurLayer = pData->page->curLayer();
    KivioLayer *pLayer = pData->page->firstLayer(); //pData->page->curLayer();

    while( pLayer )
    {
        // To be connected to, a layer must be visible and connectable
        if( pLayer!=pCurLayer )
        {
            if( pLayer->connectable()==false || pLayer->visible()==false )
            {
                pLayer = pData->page->nextLayer();
                continue;
            }
        }

        // Tell the layer to search for a target
        if( pLayer->connectPointToTarget( p, 8.0f ) )
        {
            return;
        }

        pLayer = pData->page->nextLayer();
    }


    // Nope, disconnect
    p->disconnect();
}


/**
 * Sets the position and dimensions of this stencil based on its connection points.
 */
void KivioBaseConnectorStencil::updateGeometry()
{
    double minX, minY, maxX, maxY;

    minX = 1000000000000.0f;
    minY = minX;
    maxX = -100000000000.0f;
    maxY = maxX;

    KivioConnectorPoint *p;
    p = m_pConnectorPoints->first();
    while( p )
    {
        if( p->x() < minX )
            minX = p->x();
        if( p->x() > maxX )
            maxX = p->x();
        if( p->y() < minY )
            minY = p->y();
        if( p->y() > maxY )
            maxY = p->y();

        p = m_pConnectorPoints->next();
    }

    m_x = minX;
    m_y = minY;
    m_w = maxX - minX + 1.0f;
    m_h = maxY - minY + 1.0f;
}



// file i/o routines
bool KivioBaseConnectorStencil::loadXML( const QDomElement & )
{
    return false;
}

QDomElement KivioBaseConnectorStencil::saveXML( QDomDocument & )
{
    return QDomElement();
}



KivioStencil *KivioBaseConnectorStencil::duplicate()
{
    /* Derived class must implement this function */
    return NULL;
}


bool KivioBaseConnectorStencil::boolAllTrue( bool *boolArray, int count )
{
    int i;

    for( i=0; i<count; i++ )
    {
        if( boolArray[i]==false )
            return false;
    }

    return true;
}

bool KivioBaseConnectorStencil::boolContainsFalse( bool *boolArray, int count )
{
    int i;

    for( i=0; i<count; i++ )
    {
        if( boolArray[i]==false )
            return true;
    }

    return false;
}


void KivioBaseConnectorStencil::searchForConnections( KivioPage *pPage )
{
    bool *done = new bool[ m_pConnectorPoints->count() ];

    int i;

    for( i=0; i<(int)m_pConnectorPoints->count(); i++ )
        done[i] = false;

    KivioConnectorPoint *p;

    i=0;
    p = m_pConnectorPoints->first();
    while( p )
    {
        if( p->targetId() == -1 )
            done[i] = true;

        i++;
        p = m_pConnectorPoints->next();
    }


    // No connections? BaiL!
    if( boolAllTrue( done, m_pConnectorPoints->count() ) )
    {
       delete [] done;
       return;
    }



    KivioLayer *pLayer = pPage->firstLayer();
    while( pLayer && ( boolContainsFalse(done, m_pConnectorPoints->count()) ) )
    {
        KivioStencil *pStencil = pLayer->firstStencil();

        while( pStencil && ( boolContainsFalse(done, m_pConnectorPoints->count()) ) )
        {
            // No connecting to ourself!
            if( pStencil != this )
            {

                // Iterate through all connectors attempting to connect it to the stencil.
                // If it connects, mark it as done
                i=0;
                p = m_pConnectorPoints->first();
                while( p )
                {
                    if( !done[i] &&
                         p->targetId() != -1 )
                    {
                        if( pStencil->connectToTarget( p, p->targetId() ) )
                        {
                            done[i] = true;
                        }
                    }

                    i++;
                    p = m_pConnectorPoints->next();
                }

            }

            pStencil = pLayer->nextStencil();
        }

        pLayer = pPage->nextLayer();
    }

    delete [] done;
}


//////////////////////
// resize handles
//////////////////////
int KivioBaseConnectorStencil::resizeHandlePositions()
{
    return (int)krhpNone;
}

QDomElement KivioBaseConnectorStencil::saveConnectors( QDomDocument &doc )
{
    QDomElement eConns = doc.createElement("KivioConnectors");

    KivioConnectorPoint *p;
    p = m_pConnectorPoints->first();
    while( p )
    {
        eConns.appendChild( p->saveXML(doc) );

        p = m_pConnectorPoints->next();
    }

    return eConns;
}

bool KivioBaseConnectorStencil::loadConnectors( const QDomElement &e )
{
    m_pConnectorPoints->clear();

    KivioConnectorPoint *p;

    QDomNode node = e.firstChild();
    QDomElement e2;
    QString name;

    while( !node.isNull() )
    {
        e2 = node.toElement();
        name = e2.nodeName();

        if( name == "KivioConnectorPoint" )
        {
            p = new KivioConnectorPoint();
            p->setStencil(this);
            p->loadXML( e2 );

            m_pConnectorPoints->append( p );
            p = NULL;
        }

        node = node.nextSibling();
    }

    return true;
}

QDomElement KivioBaseConnectorStencil::saveProperties( QDomDocument &doc )
{
    QDomElement propE = doc.createElement("KivioConnectorProperties");

    propE.appendChild( m_pLineStyle->saveXML( doc ) );

    propE.appendChild( m_pFillStyle->saveXML( doc ) );

    return propE;
}

bool KivioBaseConnectorStencil::loadProperties( const QDomElement &e )
{
    QDomNode node;
    QString nodeName;

    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        if( nodeName == "KivioFillStyle" )
        {
            m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }

    return true;
}
