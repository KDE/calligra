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

#include "kivio_1d_stencil.h"
#include "kivio_arrowhead.h"
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
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_text_style.h"

#include <kdebug.h>
#include <math.h>




/**
 * Default constructor.
 *
 * This will allocate a new fill style object, a list for
 * connection points, and set some default values.
 */
Kivio1DStencil::Kivio1DStencil()
    : KivioStencil()
{
    m_pFillStyle = new KivioFillStyle();
    m_pLineStyle = new KivioLineStyle();
    m_pTextStyle = new KivioTextStyle();

    m_pConnectorPoints = new QList<KivioConnectorPoint>;
    m_pConnectorPoints->setAutoDelete(true);

    m_pStart = new KivioConnectorPoint(this, true);
    m_pStart->setPosition(72.0f, 18.0f, false);

    m_pEnd = new KivioConnectorPoint(this, true);
    m_pEnd->setPosition(0.0f, 18.0f, false);

    m_pLeft = new KivioConnectorPoint(this, false);
    m_pLeft->setPosition(36.0f, 36.0f, false);

    m_pRight = new KivioConnectorPoint(this, false);
    m_pRight->setPosition(36.0f, 0.0f, false);

    m_pTextConn = new KivioConnectorPoint(this, false);
    m_pTextConn->setPosition(36.0f, 18.0f, false);

    m_connectorWidth = 36.0f;
    m_needsWidth = true;
    m_needsText = false;

    m_pConnectorPoints->append( m_pStart );
    m_pConnectorPoints->append( m_pEnd );
    m_pConnectorPoints->append( m_pLeft );
    m_pConnectorPoints->append( m_pRight );
    m_pConnectorPoints->append( m_pTextConn );
}


/**
 * Destructor
 */
Kivio1DStencil::~Kivio1DStencil()
{
   delete m_pFillStyle;
   delete m_pLineStyle;
   delete m_pTextStyle;
   delete m_pConnectorPoints;
}

void Kivio1DStencil::setFGColor( QColor c )
{
    m_pLineStyle->setColor(c);
}

QColor Kivio1DStencil::fgColor()
{
    return m_pLineStyle->color();
}

void Kivio1DStencil::setLineWidth( float f )
{
    m_pLineStyle->setWidth(f);
}

float Kivio1DStencil::lineWidth()
{
    return m_pLineStyle->width();
}

void Kivio1DStencil::setBGColor( QColor c )
{
    m_pFillStyle->setColor(c);
}

QColor Kivio1DStencil::bgColor()
{
    return m_pFillStyle->color();
}


/////////////////////////////////
// Position functions
/////////////////////////////////
void Kivio1DStencil::setX( float x )
{
    float dx = x - m_x;

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

void Kivio1DStencil::setY( float y )
{
    float dy = y - m_y;

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

void Kivio1DStencil::setPosition( float x, float y )
{
    float dx = x - m_x;
    float dy = y - m_y;

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
void Kivio1DStencil::setStartPoint( float x, float y )
{
   float oldX = m_pStart->x();
   float oldY = m_pStart->y();

   m_pStart->setPosition(x, y, false);
   m_pStart->disconnect();

   updateConnectorPoints(m_pStart, oldX, oldY);
}


void Kivio1DStencil::setEndPoint( float x, float y )
{
   float oldX = m_pEnd->x();
   float oldY = m_pEnd->y();

   m_pEnd->setPosition(x, y, false);
   m_pEnd->disconnect();

   updateConnectorPoints(m_pEnd, oldX, oldY);
}

void Kivio1DStencil::updateConnectorPoints( KivioConnectorPoint *p, float /*oldX*/, float /*oldY*/ )
{
   // If p is the start or end, we need to adjust the width connectors
   if( p == m_pStart || p == m_pEnd )
   {
      float vx = m_pStart->x() - m_pEnd->x();
      float vy = m_pStart->y() - m_pEnd->y();
      float len = sqrt( vx*vx + vy*vy );
      float midX = (m_pStart->x() + m_pEnd->x())/2.0f;
      float midY = (m_pStart->y() + m_pEnd->y())/2.0f;
      
      vx /= len;
      vy /= len;
      
      float d = m_connectorWidth/2.0f;
      
      m_pLeft->setPosition( midX + d*vy, midY + d*(-vx), false );
      m_pRight->setPosition( midX + d*(-vy), midY + d*vx, false );  
   }

   updateGeometry();
}


void Kivio1DStencil::paint( KivioIntraStencilData * )
{
    /* Derived class must implement this */
}

void Kivio1DStencil::paintOutline( KivioIntraStencilData *pData )
{
    /* Derived class should implement this */
    paint( pData );
}

void Kivio1DStencil::paintConnectorTargets( KivioIntraStencilData * )
{
}

void Kivio1DStencil::paintSelectionHandles( KivioIntraStencilData *pData )
{
    KivioPainter *painter = pData->painter;
    float x1, y1, scale;
    int flag;

    scale = pData->scale;

    KivioConnectorPoint *p = m_pConnectorPoints->first();
    while( p )
    {
       // If we don't need width connectors and we are on a width connector,
       // ignore it.
       x1 = p->x() * scale;
       y1 = p->y() * scale;
       
       flag = (p->target()) ? KivioPainter::cpfConnected : 0;
       
       if( p==m_pTextConn )
       {
	  if( m_needsText==true )
	  {
	     painter->drawHandle( x1, y1, 0 );
	  }
       }
       else if( p==m_pLeft || p==m_pRight )
       {
	  if( m_needsWidth==true )
	  {
	     painter->drawHandle( x1, y1, 0 );
	  }
       }
       else if( p==m_pStart )
       {
	  painter->drawHandle( x1, y1, KivioPainter::cpfStart | flag );
       }
       else if( p==m_pEnd )
       {
	  painter->drawHandle( x1, y1, KivioPainter::cpfEnd | flag );
       }
       else
       {
	  if( p->connectable() )
	     painter->drawHandle( x1, y1, KivioPainter::cpfConnectable | flag );
	  else
	     painter->drawHandle( x1, y1, flag );
       }

        p = m_pConnectorPoints->next();
    }
}



///////////////////////////////
// Collision detection
///////////////////////////////
KivioCollisionType Kivio1DStencil::checkForCollision( KivioPoint *, float )
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
void Kivio1DStencil::customDrag( KivioCustomDragData *pData )
{
    float _x = pData->x;
    float _y = pData->y;
    int id = pData->id;
    float oldX, oldY;
    bool doneSearching = false;
    bool foundConnection = false; 

    float oldStencilX, oldStencilY;

    

    KivioConnectorPoint *p;


    // Locate the point specified by id
    p = m_pConnectorPoints->at( id - (kctCustom+1));


    if( !p )
    {
       kdDebug() << "Kivio1DStencil::customDrag() - KivioConnectorPoint id: " << (id - (kctCustom+1)) << "  not found\n" << endl;
       return;
    }

    oldX = p->x();
    oldY = p->y();
    p->setPosition(_x,_y,true);

    if( p->connectable()==true )
    {
       // Attempt a snap....
       KivioLayer *pCurLayer = pData->page->curLayer();
       KivioLayer *pLayer = pData->page->firstLayer(); //pData->page->curLayer();
       
       while( pLayer && doneSearching==false)
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
	     foundConnection = true;
	     doneSearching = true;
	  }
	  
	  pLayer = pData->page->nextLayer();
       }
       
       if( foundConnection == false )
       {
	  p->disconnect();
       }
    }

    
    // If it is a start/end point, then make a request to update the connectors (must be implemented by stencil developer)
    if( id == kctCustom+1 ||
	id == kctCustom+2 )
    {
       // If it's the end connector, then update the text point
       if( p==m_pEnd && m_needsText==true )
       {
	  m_pTextConn->setPosition( m_pTextConn->x() + (m_pEnd->x() - oldX),
			            m_pTextConn->y() + (m_pEnd->y() - oldY),
				    false );
       }

       updateConnectorPoints(p, oldX, oldY);
    }
    // If it is one of the width handles, then fix the width and update the opposite point
    // only if the stencils 'needs width' connectors
    else if( (id == kctCustom+3 || id == kctCustom+4) &&
             (m_needsWidth==true) )
    {
       float vx = m_pStart->x() - m_pEnd->x();
       float vy = m_pStart->y() - m_pEnd->y();
       float len = sqrt( vx*vx + vy*vy );
       float midX = (m_pStart->x() + m_pEnd->x())/2.0f;
       float midY = (m_pStart->y() + m_pEnd->y())/2.0f;

       vx /= len;
       vy /= len;

       float d = shortestDistance( m_pStart, m_pEnd, (id==kctCustom+3) ? m_pLeft : m_pRight );

       m_pLeft->setPosition( midX + d*vy, midY + d*(-vx), false );
       m_pRight->setPosition( midX + d*(-vy), midY + d*vx, false );

       m_connectorWidth = d*2.0f;

       updateConnectorPoints(p, oldX, oldY);
       return;
    }
    // Text handle
    else if( id == kctCustom+5 )
    {
       updateConnectorPoints(p, oldX, oldY);
    }
}


/**
 * Sets the position and dimensions of this stencil based on its connection points.
 */
void Kivio1DStencil::updateGeometry()
{
    float minX, minY, maxX, maxY;

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
bool Kivio1DStencil::loadXML( const QDomElement &e )
{
   QDomNode node;
   QString name;

   node = e.firstChild();
   while( !node.isNull() )
   {
      name = node.nodeName();

      if( name == "KivioStencilProperties" )
      {
	 loadProperties(node.toElement() );
      }

      node = node.nextSibling();
   }

   updateGeometry();

   return true;
}


QDomElement Kivio1DStencil::createRootElement( QDomDocument &doc )
{
   QDomElement e = doc.createElement("KivioPluginStencil");

   XmlWriteString( e, "title", m_pSpawner->info()->title() );
   XmlWriteString( e, "setName", m_pSpawner->set()->name() );

   return e;
}


QDomElement Kivio1DStencil::saveXML( QDomDocument &doc )
{
   QDomElement e = createRootElement(doc);

   e.appendChild( saveProperties(doc) );

   return e;
}



KivioStencil *Kivio1DStencil::duplicate()
{
    /* Derived class must implement this function */
    return NULL;
}


bool Kivio1DStencil::boolAllTrue( bool *boolArray, int count )
{
    int i;

    for( i=0; i<count; i++ )
    {
        if( boolArray[i]==false )
            return false;
    }

    return true;
}

bool Kivio1DStencil::boolContainsFalse( bool *boolArray, int count )
{
    int i;

    for( i=0; i<count; i++ )
    {
        if( boolArray[i]==false )
            return true;
    }

    return false;
}


void Kivio1DStencil::searchForConnections( KivioPage *pPage )
{
    bool *done = new bool[ m_pConnectorPoints->count()];

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
int Kivio1DStencil::resizeHandlePositions()
{
    return (int)krhpNone;
}

QDomElement Kivio1DStencil::saveConnectors( QDomDocument &doc )
{
    QDomElement eConns = doc.createElement("KivioConnectorList");

    KivioConnectorPoint *p;
    p = m_pConnectorPoints->first();
    while( p )
    {
        eConns.appendChild( p->saveXML(doc) );

        p = m_pConnectorPoints->next();
    }

    return eConns;
}

bool Kivio1DStencil::loadConnectors( const QDomElement &e )
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

    // Set the pointers to the start,end,left,right points
    m_pStart = m_pConnectorPoints->first();
    m_pEnd = m_pConnectorPoints->next();
    m_pLeft = m_pConnectorPoints->next();
    m_pRight = m_pConnectorPoints->next();
    m_pTextConn = m_pConnectorPoints->next();

    // Hopefully this will help with backwards compatibility
    if( m_pStart == NULL ) {
       m_pStart = new KivioConnectorPoint(this, true);
    }
    if( m_pEnd == NULL ) {
       m_pEnd = new KivioConnectorPoint(this, true);
    }
    if( m_pLeft == NULL ) {
       m_pLeft = new KivioConnectorPoint(this, false);
    }
    if( m_pRight == NULL ) {
       m_pRight = new KivioConnectorPoint(this, false);
    }
    if( m_pTextConn == NULL ) {
       m_pTextConn = new KivioConnectorPoint(this, false);
    }
    

    return true;
}

QDomElement Kivio1DStencil::saveProperties( QDomDocument &doc )
{
    QDomElement propE = doc.createElement("KivioStencilProperties");

    QDomElement connE = doc.createElement("Kivio1DProperties");
    XmlWriteFloat( connE, "connectorWidth", m_connectorWidth );
    XmlWriteInt( connE, "needsWidth", m_needsWidth );
    propE.appendChild( connE );

    propE.appendChild( m_pLineStyle->saveXML( doc ) );

    propE.appendChild( m_pFillStyle->saveXML( doc ) );

    propE.appendChild( m_pTextStyle->saveXML( doc ) );

    propE.appendChild( saveConnectors(doc) );

    QDomElement customE = doc.createElement("CustomData");
    if( saveCustom( customE, doc )==true )
    {
       propE.appendChild( customE );
    }

    return propE;
}

bool Kivio1DStencil::loadProperties( const QDomElement &e )
{
    QDomNode node;
    QDomElement nodeE;
    QString nodeName;

    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeE = node.toElement();
        nodeName = node.nodeName();

        if( nodeName == "KivioFillStyle" )
        {
            m_pFillStyle->loadXML( nodeE );
        }
        else if( nodeName == "KivioLineStyle" )
        {
	   m_pLineStyle->loadXML( nodeE );
        }
	else if( nodeName == "KivioTextStyle" )
	{
	   m_pTextStyle->loadXML( nodeE );
	}
	else if( nodeName == "KivioConnectorList" )
	{
	   loadConnectors( nodeE );
	}
	else if( nodeName == "Kivio1DProperties" )
	{
	   m_needsWidth = (bool)XmlReadInt( nodeE, "needsWidth", (int)true );
	   m_connectorWidth = XmlReadFloat( nodeE, "connectorWidth", 36.0f );
	}
	else if( nodeName == "CustomData" )
	{
	   loadCustom( nodeE );
	}

        node = node.nextSibling();
    }

    return true;
}

bool Kivio1DStencil::loadCustom( const QDomElement & )
{
   return true;
}

bool Kivio1DStencil::saveCustom( QDomElement &, QDomDocument & )
{
   return false;
}

void Kivio1DStencil::copyBasicInto( Kivio1DStencil *pStencil )
{
    KivioConnectorPoint *pSrc, *pTg;

    // Copy the spawner
    pStencil->setSpawner( m_pSpawner );

    // Copy the connector points
    pSrc = m_pConnectorPoints->first();
    pTg = pStencil->m_pConnectorPoints->first();
    while( pSrc && pTg )
    {
       pTg->setPosition( pSrc->x(), pSrc->y(), false );

       pSrc = m_pConnectorPoints->next();
       pTg = pStencil->m_pConnectorPoints->next();
    }

    // Copy the dimensions
    pStencil->m_x = m_x;
    pStencil->m_y = m_y;
    pStencil->m_w = m_w;
    pStencil->m_h = m_h;

    // Copy over the width info
    pStencil->m_connectorWidth = m_connectorWidth;
    pStencil->m_needsWidth = m_needsWidth;

    // Copy over the styles
    m_pFillStyle->copyInto( pStencil->m_pFillStyle );
    m_pLineStyle->copyInto( pStencil->m_pLineStyle );
    m_pTextStyle->copyInto( pStencil->m_pTextStyle );

    // Copy over the protection
    *(pStencil->m_pProtection) = *m_pProtection;
    *(pStencil->m_pCanProtect) = *m_pCanProtect;
}

void Kivio1DStencil::drawText( KivioIntraStencilData *pData )
{
   float scale = pData->scale;
   KivioPainter *painter = pData->painter;

   int tf;
   float _x, _y, _w, _h;
   QRect boundRect;

   _x = m_pTextConn->x() * scale;
   _y = m_pTextConn->y() * scale;
   _w = 10000000.0f;
   _h = 10000000.0f;

   QFont f = m_pTextStyle->font();

   f.setPointSize( f.pointSize() * scale);
   painter->setFont(f);
   painter->setTextColor(m_pTextStyle->color());
   

   tf = m_pTextStyle->hTextAlign() | m_pTextStyle->vTextAlign();

   boundRect = painter->boundingRect( (int)_x, (int)_y, (int)_w, (int)_h, tf, m_pTextStyle->text() );

   painter->drawText( _x, _y, boundRect.width(), boundRect.height(), tf, m_pTextStyle->text() );
}

