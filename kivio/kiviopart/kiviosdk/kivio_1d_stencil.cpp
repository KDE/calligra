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
#include "kivio_connector_target.h"

#include <kdebug.h>
#include <math.h>
#include <KoZoomHandler.h>

#include <qbitmap.h>
#include <qpainter.h>

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

    m_pConnectorPoints = new QPtrList<KivioConnectorPoint>;
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

void Kivio1DStencil::setLineWidth( double f )
{
    m_pLineStyle->setWidth(f);
}

double Kivio1DStencil::lineWidth()
{
    return m_pLineStyle->width();
}

void Kivio1DStencil::setLinePattern(int p)
{
    m_pLineStyle->setStyle(p);
}

int Kivio1DStencil::linePattern()
{
    return m_pLineStyle->style();
}

void Kivio1DStencil::setFillPattern(int p)
{
    m_pFillStyle->setBrushStyle(static_cast<Qt::BrushStyle>(p));
}

int Kivio1DStencil::fillPattern()
{
    return m_pFillStyle->brushStyle();
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
void Kivio1DStencil::setX( double x )
{
  double dx = x - m_x;
  KivioConnectorPoint *p = m_pConnectorPoints->first();

  while( p )
  {
    p->disconnect();
    p->setX( p->x() + dx, false );
    p = m_pConnectorPoints->next();
  }

  m_x = x;
}

void Kivio1DStencil::setY( double y )
{
  double dy = y - m_y;
  KivioConnectorPoint *p = m_pConnectorPoints->first();

  while( p )
  {
    p->disconnect();
    p->setY( p->y() + dy, false );
    p = m_pConnectorPoints->next();
  }

  m_y = y;
}

void Kivio1DStencil::setPosition( double x, double y )
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
void Kivio1DStencil::setStartPoint( double x, double y )
{
   double oldX = m_pStart->x();
   double oldY = m_pStart->y();

   m_pStart->setPosition(x, y, false);
   m_pStart->disconnect();

   updateConnectorPoints(m_pStart, oldX, oldY);
}


void Kivio1DStencil::setEndPoint( double x, double y )
{
   double oldX = m_pEnd->x();
   double oldY = m_pEnd->y();

   m_pEnd->setPosition(x, y, false);
   m_pEnd->disconnect();

   updateConnectorPoints(m_pEnd, oldX, oldY);
}

void Kivio1DStencil::updateConnectorPoints( KivioConnectorPoint *p, double /*oldX*/, double /*oldY*/ )
{
   // If p is the start or end, we need to adjust the width connectors
   if( p == m_pStart || p == m_pEnd )
   {
      double vx = m_pStart->x() - m_pEnd->x();
      double vy = m_pStart->y() - m_pEnd->y();
      double len = sqrt( vx*vx + vy*vy );
      double midX = (m_pStart->x() + m_pEnd->x())/2.0f;
      double midY = (m_pStart->y() + m_pEnd->y())/2.0f;

      vx /= len;
      vy /= len;

      double d = m_connectorWidth/2.0f;

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
  double x1, y1;
  int flag;

  KoZoomHandler* zoomHandler = pData->zoomHandler;

  KivioConnectorPoint *p = m_pConnectorPoints->first();
  while( p )
  {
    // If we don't need width connectors and we are on a width connector,
    // ignore it.
    x1 = zoomHandler->zoomItX(p->x());
    y1 = zoomHandler->zoomItY(p->y());

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
      if( p->connectable() ) {
        painter->drawHandle( x1, y1, KivioPainter::cpfConnectable | flag );
      } else {
        painter->drawHandle( x1, y1, flag );
      }
    }

    p = m_pConnectorPoints->next();
  }
}



///////////////////////////////
// Collision detection
///////////////////////////////
KivioCollisionType Kivio1DStencil::checkForCollision( KoPoint *, double )
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
  setCustomIDPoint(pData->id, KoPoint(pData->x, pData->y), pData->page);
}


/**
 * Sets the position and dimensions of this stencil based on its connection points.
 */
void Kivio1DStencil::updateGeometry()
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

   XmlWriteString( e, "id", m_pSpawner->info()->id() );
   XmlWriteString( e, "setId", m_pSpawner->set()->id() );

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
  unsigned int i;

  for(i = 0; i < m_pConnectorPoints->count(); i++) {
    done[i] = false;
  }

  KivioConnectorPoint *p;
  i = 0;
  p = m_pConnectorPoints->first();

  while( p )
  {
    if( p->targetId() == -1 ) {
      done[i] = true;
    }

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
      if((pStencil != this))
      {

        // Iterate through all connectors attempting to connect it to the stencil.
        // If it connects, mark it as done
        i=0;
        p = m_pConnectorPoints->first();
        while( p )
        {
          if( !done[i] && p->targetId() != -1 )
          {
            if(pStencil->connectToTarget( p, p->targetId()))
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

void Kivio1DStencil::searchForConnections( KivioPage *pPage, double threshold )
{
  bool *done = new bool[ m_pConnectorPoints->count()];
  int i;

  for( i=0; i<(int)m_pConnectorPoints->count(); i++ ) {
    done[i] = false;
  }

  KivioConnectorPoint *p;
  i = 0;
  p = m_pConnectorPoints->first();

  while( p )
  {
    if(p->target() != 0L) {
      done[i] = true;
    }

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
        i = 0;
        p = m_pConnectorPoints->first();

        while( p )
        {
          if( !done[i] && p->target() == 0 )
          {
            if( pStencil->connectToTarget( p, threshold ) )
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
  if(m_pTextStyle->text().isEmpty()) {
    return;
  }
  
  KoZoomHandler* zoomHandler = pData->zoomHandler;
  KivioPainter *painter = pData->painter;

  int _x, _y, _w, _h;

  _x = zoomHandler->zoomItX(m_pTextConn->x());
  _y = zoomHandler->zoomItY(m_pTextConn->y());
  _w = 10000000;
  _h = 10000000;

  QFont f = m_pTextStyle->font();
  int tf = m_pTextStyle->hTextAlign() | m_pTextStyle->vTextAlign();

  f.setPointSizeFloat(f.pointSizeFloat() * (((float)zoomHandler->zoom()) / 100.0));
  painter->setFont(f);
  QRect boundRect = painter->boundingRect( _x, _y, _w, _h, tf, m_pTextStyle->text() );
  QPixmap pix(boundRect.width(), boundRect.height());
  pix.fill();
  QPainter p(&pix);
  p.setPen(m_pTextStyle->color());
  p.setFont(f);
  p.drawText( 0, 0, boundRect.width(), boundRect.height(), tf, m_pTextStyle->text() );
  QBitmap mask;
  mask = pix;
  pix.setMask(mask);
  painter->drawPixmap(_x, _y, pix);
}

bool Kivio1DStencil::connected()
{
  KivioConnectorPoint *p;
  p = m_pConnectorPoints->first();

  while( p )
  {
    if(p->target() != 0) {
      return true;
    }

    p = m_pConnectorPoints->next();
  }

  return false;
}

void Kivio1DStencil::disconnectFromTargets()
{
  KivioConnectorPoint *p;
  p = m_pConnectorPoints->first();

  while( p )
  {
    p->disconnect(true);
    p = m_pConnectorPoints->next();
  }
}

KivioLineStyle Kivio1DStencil::lineStyle()
{
  return *m_pLineStyle;
}

void Kivio1DStencil::setLineStyle(KivioLineStyle ls)
{
  ls.copyInto(m_pLineStyle);
}

void Kivio1DStencil::setCustomIDPoint(int customID, const KoPoint& point, KivioPage* page)
{
  double oldX, oldY;
  KivioConnectorPoint *p;

  // Locate the point specified by customID
  p = m_pConnectorPoints->at( customID - (kctCustom+1));

  if( !p )
  {
    kdDebug(43000) << "Kivio1DStencil::customDrag() - KivioConnectorPoint customID: " << (customID - (kctCustom+1)) << "  not found\n" << endl;
    return;
  }

  oldX = p->x();
  oldY = p->y();
  p->setPosition(point.x(),point.y(),true);

  if( p->connectable()==true )
  {
    // Attempt a snap....
    KivioLayer *pCurLayer = page->curLayer();
    KivioLayer *pLayer = page->firstLayer(); //page->curLayer();
    bool foundConnection = false;

    while( pLayer && !foundConnection )
    {
      // To be connected to, a layer must be visible and connectable
      if( pLayer!=pCurLayer )
      {
        if( pLayer->connectable()==false || pLayer->visible()==false )
        {
          pLayer = page->nextLayer();
          continue;
        }
      }

      // Tell the layer to search for a target
      if( pLayer->connectPointToTarget( p, 8.0f ) )
      {
        foundConnection = true;
      }

      pLayer = page->nextLayer();
    }

    if( foundConnection == false )
    {
      p->disconnect();
    }
  }

  // If it is a start/end point, then make a request to update the connectors (must be implemented by stencil developer)
  if( customID == kctCustom+1 || customID == kctCustom+2 )
  {
    // If it's the end connector, then update the text point
    if( p==m_pEnd && m_needsText==true )
    {
      m_pTextConn->setPosition( m_pTextConn->x() + (m_pEnd->x() - oldX),
                                m_pTextConn->y() + (m_pEnd->y() - oldY), false );
    }

    updateConnectorPoints(p, oldX, oldY);
  }
  // If it is one of the width handles, then fix the width and update the opposite point
  // only if the stencils 'needs width' connectors
  else if( (customID == kctCustom+3 || customID == kctCustom+4) && (m_needsWidth==true) )
  {
    double vx = m_pStart->x() - m_pEnd->x();
    double vy = m_pStart->y() - m_pEnd->y();
    double len = sqrt( vx*vx + vy*vy );
    double midX = (m_pStart->x() + m_pEnd->x())/2.0f;
    double midY = (m_pStart->y() + m_pEnd->y())/2.0f;

    vx /= len;
    vy /= len;

    double d = shortestDistance( m_pStart, m_pEnd, (customID==kctCustom+3) ? m_pLeft : m_pRight );

    m_pLeft->setPosition( midX + d*vy, midY + d*(-vx), false );
    m_pRight->setPosition( midX + d*(-vy), midY + d*vx, false );

    m_connectorWidth = d*2.0f;

    updateConnectorPoints(p, oldX, oldY);
    return;
  }
  // Text handle
  else if( customID == kctCustom+5 )
  {
    updateConnectorPoints(p, oldX, oldY);
  }
}

KoPoint Kivio1DStencil::customIDPoint(int customID)
{
  KivioConnectorPoint *p;

  // Locate the point specified by customID
  p = m_pConnectorPoints->at( customID - (kctCustom+1));

  return p->position();
}
