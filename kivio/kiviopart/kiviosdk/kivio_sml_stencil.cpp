/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti
 *                         Peter Simonsson
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
#include "kivioglobal.h"
#include "kivio_common.h"
#include "kivio_connector_point.h"
#include "kivio_connector_target.h"
#include "kivio_fill_style.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_screen_painter.h"
#include "kivio_shape.h"
#include "kivio_shape_data.h"
#include "kivio_sml_stencil.h"
#include "kivio_dia_stencil_spawner.h"
#include "kivio_sml_stencil_spawner.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"

#include <qdom.h>
#include <qpainter.h>
#include <qbrush.h>
#include <QColor>
#include <qpalette.h>
#include <qrect.h>
#include <qbitmap.h>
#include <kdebug.h>
#include <KoGlobal.h>
#include <math.h>
#include <KoZoomHandler.h>

/**
 * Default constructor
 *
 * Allocates a new shape list, connector targets, and original connector targets.
 */
KivioSMLStencil::KivioSMLStencil()
    : KivioStencil(),
      m_pShapeList(NULL),
      m_pSubSelection(NULL),
      m_pConnectorTargets(NULL)
{
    m_pShapeList = new QPtrList<KivioShape>;
    m_pShapeList->setAutoDelete(true);

    m_pConnectorTargets = new QPtrList<KivioConnectorTarget>;
    m_pConnectorTargets->setAutoDelete(true);
}


/**
 * Destructor
 *
 * Deletes the allocated objects.
 */
KivioSMLStencil::~KivioSMLStencil()
{
    delete m_pShapeList;
    m_pShapeList = 0;
    
    delete m_pConnectorTargets;
    m_pConnectorTargets = 0;
    
    m_pSubSelection = 0;
}


/**
 * Loads a KivioSMLStencil from an XML node.
 */
bool KivioSMLStencil::loadXML( const QDomElement &e )
{
    QDomNode node;
    QDomElement ele;


    node = e.firstChild();
    while( !node.isNull() )
    {
        QString nodeName = node.nodeName();

        ele = node.toElement();

        if( nodeName == "Position" )
        {
            m_x = XmlReadFloat( ele, "x", 0.0f );
            m_y = XmlReadFloat( ele, "y", 0.0f );
        }
        else if( nodeName == "Dimension" )
        {
            m_w = XmlReadFloat( ele, "w", 0.0f );
            m_h = XmlReadFloat( ele, "h", 0.0f );
        }
        else if( nodeName == "KivioShape" )
        {
            // Locate the shape we are supposed to load into
            KivioShape *pShape = locateShape( XmlReadString( ele, "name", "" ) );

            if(pShape) {
              pShape->loadXML( ele );
            }
        }
        else if( nodeName == "KivioConnectorTargetList" )
        {
            loadConnectorTargetListXML( ele );
        }

        node = node.nextSibling();
    }
    return true;
}

/**
 * Help function for loading from an XML node.
 */
void KivioSMLStencil::loadConnectorTargetListXML( const QDomElement &e )
{
  QDomNode node;
  QDomElement ele;
  QString nodeName;
  KivioConnectorTarget *pTarget;

  pTarget = m_pConnectorTargets->first();
  node = e.firstChild();
  while( !node.isNull() && pTarget)
  {
    nodeName = node.nodeName();
    ele = node.toElement();

    if( nodeName == "KivioConnectorTarget" )
    {
      pTarget->loadXML( ele );
      pTarget->setOffsets((pTarget->x() - x()) / w(), (pTarget->y() - y()) / h());
      pTarget = m_pConnectorTargets->next();
    }

    node = node.nextSibling();
  }
  
  while(!node.isNull()) {
    nodeName = node.nodeName();
    ele = node.toElement();

    if( nodeName == "KivioConnectorTarget" )
    {
      pTarget = new KivioConnectorTarget();
      pTarget->loadXML( ele );
      pTarget->setOffsets((pTarget->x() - x()) / w(), (pTarget->y() -y()) / h());
      m_pConnectorTargets->append(pTarget);
    }

    node = node.nextSibling();
  }
}


/**
 * Locates a shape in the shape list by name.
 */
KivioShape *KivioSMLStencil::locateShape( const QString &name )
{
    KivioShape *pShape;

    if( name.isEmpty() )
        return NULL;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->name() == name )
        {
            return pShape;
        }

        pShape = m_pShapeList->next();
    }

    return NULL;
}


/**
 * Saves this object to an XMLELement
 */
QDomElement KivioSMLStencil::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioSMLStencil");

    XmlWriteString( e, "id", m_pSpawner->info()->id() );
    XmlWriteString( e, "setId", m_pSpawner->set()->id() );


    // The positions
    QDomElement posE = doc.createElement("Position");
    XmlWriteFloat( posE, "x", m_x );
    XmlWriteFloat( posE, "y", m_y );
    e.appendChild( posE );

    // The dimensions
    QDomElement dimE = doc.createElement("Dimension");
    XmlWriteFloat( dimE, "w", m_w );
    XmlWriteFloat( dimE, "h", m_h );
    e.appendChild( dimE );

    // Save the target list
    QDomElement clE = doc.createElement("KivioConnectorTargetList");
    QDomElement targetE;
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        targetE = pTarget->saveXML( doc );
        clE.appendChild( targetE );

        pTarget = m_pConnectorTargets->next();
    }
    e.appendChild( clE );

    // The shape list
    KivioShape *pShape = m_pShapeList->first();
    while( pShape )
    {
        e.appendChild( pShape->saveXML(doc ) );

        pShape = m_pShapeList->next();
    }

    return e;
}


/**
 * Duplicates this object.
 *
 * Duplicates all aspects of this object except for the
 * stencil connected to the targets.
 */
KivioStencil *KivioSMLStencil::duplicate()
{
    KivioSMLStencil *pNewStencil = new KivioSMLStencil();
    KivioStencil *pReturn;
    KivioShape *pShape, *pNewShape;

    pNewStencil->m_x = m_x;
    pNewStencil->m_y = m_y;
    pNewStencil->m_w = m_w;
    pNewStencil->m_h = m_h;

    pNewStencil->m_rotation = m_rotation;

    pNewStencil->m_pSpawner = m_pSpawner;


    // Copy the shape list
    pShape = m_pShapeList->first();
    while( pShape )
    {
        pNewShape = new KivioShape( *pShape );
        pNewStencil->m_pShapeList->append( pNewShape );

        pShape = m_pShapeList->next();
    }

    // Copy the Connector Targets
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    while( pTarget ) //&& pOriginal )
    {
        pNewStencil->m_pConnectorTargets->append( pTarget->duplicate() );
        pTarget = m_pConnectorTargets->next();
    }

    *(pNewStencil->protection()) = *m_pProtection;
    *(pNewStencil->canProtect()) = *m_pCanProtect;


    pReturn = pNewStencil;

    return pReturn;
}


/**
 * Paint the outline of the stencil.
 */
void KivioSMLStencil::paintOutline( KivioIntraStencilData *pData )
{
    KivioShape *pShape;
    KivioShapeData *pShapeData;

    m_zoomHandler = pData->zoomHandler;
    pData->painter->saveState();
    pData->painter->setTranslation(m_zoomHandler->zoomItX(m_x), m_zoomHandler->zoomItY(m_y));
    rotatePainter(pData);  // Rotate the painter if needed

    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShapeData = pShape->shapeData();

        switch( pShapeData->shapeType() )
        {
            case KivioShapeData::kstArc:
                drawOutlineArc( pShape, pData );
                break;

            case KivioShapeData::kstPie:
                drawOutlinePie( pShape, pData );
                break;

            case KivioShapeData::kstLineArray:
                drawOutlineLineArray( pShape, pData );
                break;

            case KivioShapeData::kstPolyline:
                drawOutlinePolyline( pShape, pData );
                break;

            case KivioShapeData::kstPolygon:
                drawOutlinePolygon( pShape, pData );
                break;

            case KivioShapeData::kstBezier:
                drawOutlineBezier( pShape, pData );
                break;

            case KivioShapeData::kstRectangle:
                drawOutlineRectangle( pShape, pData );
                break;

            case KivioShapeData::kstRoundRectangle:
                drawOutlineRoundRectangle( pShape, pData );
                break;

            case KivioShapeData::kstEllipse:
                drawOutlineEllipse( pShape, pData );
                break;

            case KivioShapeData::kstOpenPath:
                drawOutlineOpenPath( pShape, pData );
                break;

            case KivioShapeData::kstClosedPath:
                drawOutlineClosedPath( pShape, pData );
                break;

            case KivioShapeData::kstTextBox:
                drawOutlineTextBox( pShape, pData );
                break;


            case KivioShapeData::kstNone:
            default:
                kDebug(43000) << "*** KivioShape::Paint AHHHHH!!! NO SHAPE!" << endl;
                break;
        }

        pShape = m_pShapeList->next();
    }
    pData->painter->restoreState();

    // Now iterate through anything connected to it drawing it as an outline
    KivioConnectorTarget *pTarget;

    pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        pTarget->paintOutline( pData );

        pTarget = m_pConnectorTargets->next();
    }
}

void KivioSMLStencil::drawOutlineArc( KivioShape *pShape, KivioIntraStencilData *pData )
{
    double _a, _l, _x, _y, _w, _h, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;
    KivioPoint *pPoint;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
    _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
    _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

    pPoint = pShapeData->pointList()->first();
    _a = m_zoomHandler->zoomItX(pPoint->x());
    _l = m_zoomHandler->zoomItY(pPoint->y());


    painter = pData->painter;
    painter->drawArc( _x, _y, _w, _h, _a, _l );
}

void KivioSMLStencil::drawOutlineBezier( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();


  painter = pData->painter;
  pShapeData = pShape->shapeData();

  KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPointArray controlPoints( 4 );


  pPoint = pPointList->first();
  pPoint2 = pPointList->next();
  pPoint3 = pPointList->next();
  pPoint4 = pPointList->next();


  controlPoints.setPoint( 0, m_zoomHandler->zoomItX((pPoint->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h) );
  controlPoints.setPoint( 1, m_zoomHandler->zoomItX((pPoint2->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint2->y()/defHeight)*m_h));
  controlPoints.setPoint( 2, m_zoomHandler->zoomItX((pPoint3->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint3->y()/defHeight)*m_h));
  controlPoints.setPoint( 3, m_zoomHandler->zoomItX((pPoint4->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint4->y()/defHeight)*m_h));

  painter = pData->painter;
  painter->drawBezier( controlPoints );
}

void KivioSMLStencil::drawOutlineOpenPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPoint, *pNewPoint;

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();

  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
  pNewPoints->setAutoDelete(true);

  pPoint = pPointList->first();

  while( pPoint )
  {
    pNewPoint = new KivioPoint( m_zoomHandler->zoomItX((pPoint->x()/defWidth)*m_w),
                                m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h),
                                pPoint->pointType() );
    pNewPoints->append(pNewPoint);

    pPoint = pPointList->next();
  }

  painter = pData->painter;
  painter->drawOpenPath( pNewPoints );

  delete pNewPoints;
}

void KivioSMLStencil::drawOutlineClosedPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPoint, *pNewPoint;

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();

  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
  pNewPoints->setAutoDelete(true);

  pPoint = pPointList->first();
  while( pPoint )
  {
    pNewPoint = new KivioPoint( m_zoomHandler->zoomItX((pPoint->x()/defWidth)*m_w),
                                m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h),
                                pPoint->pointType() );
    pNewPoints->append(pNewPoint);

    pPoint = pPointList->next();
  }

  painter = pData->painter;
  painter->drawOpenPath( pNewPoints );

  delete pNewPoints;
}

void KivioSMLStencil::drawOutlineEllipse( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _x, _y, _w, _h, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;


  painter = pData->painter;
  painter->setFGColor( QColor(0, 0, 0) );
  painter->drawEllipse( _x, _y, _w, _h );
}

void KivioSMLStencil::drawOutlineLineArray( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _x, _y, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    arr.setPoint( i, (int)_x, (int)_y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->drawLineArray( arr );
}

void KivioSMLStencil::drawOutlineRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _x, _y, _w, _h, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

  painter = pData->painter;
  painter->setFGColor( QColor(0, 0, 0) );
  painter->drawRect( _x, _y, _w, _h );
}

void KivioSMLStencil::drawOutlineRoundRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _rx, _ry, _x, _y, _w, _h, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  KivioPoint *pPoint;

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pPoint = pShapeData->pointList()->first();
  _rx = m_zoomHandler->zoomItX(pPoint->x());
  _ry = m_zoomHandler->zoomItY(pPoint->y());


  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;


  painter = pData->painter;
  painter->setFGColor( QColor(0, 0, 0) );
  painter->drawRoundRect( _x, _y, _w, _h, _rx, _ry );
}

void KivioSMLStencil::drawOutlinePie( KivioShape *, KivioIntraStencilData * )
{

}

void KivioSMLStencil::drawOutlinePolygon( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _x, _y, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    arr.setPoint( i, (int)_x, (int)_y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->drawPolyline( arr );
}

void KivioSMLStencil::drawOutlinePolyline( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double _x, _y, defWidth, defHeight;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();

  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    arr.setPoint( i, (int)_x, (int)_y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->drawPolyline( arr );
}

void KivioSMLStencil::drawOutlineTextBox( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();
  int _x, _y, _w, _h;
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPosition = pShapeData->position();
  KivioPoint *pDimensions = pShapeData->dimensions();
  KivioPainter *painter = pData->painter;
  KoZoomHandler* zoomHandler = pData->zoomHandler;

  if( pShapeData->text().length() <= 0 ) {
    return;
  }


  _x = zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;


  QPixmap pix(_w, _h);
  pix.fill();
  QPainter p(&pix);
  
  QFont f = pShapeData->textFont();
  f.setPointSizeFloat(f.pointSizeFloat() * (((float)zoomHandler->zoom()) / 100.0));
  p.setFont( f );
  p.setPen( QColor(0, 0, 0) );
  int tf = pShapeData->vTextAlign() | pShapeData->hTextAlign();
  p.drawText( 0, 0, _w, _h, tf | Qt::TextWordWrap, pShapeData->text() );
  QBitmap mask;
  mask = pix;
  pix.setMask(mask);
  painter->drawPixmap(_x, _y, pix);
}


/**
 * Paints the stencil
 */
void KivioSMLStencil::paint( KivioIntraStencilData *pData )
{
  KivioShape *pShape;
  KivioShapeData *pShapeData;


  m_zoomHandler = pData->zoomHandler;
  pData->painter->saveState();
  pData->painter->setTranslation(m_zoomHandler->zoomItX(m_x), m_zoomHandler->zoomItY(m_y));
  rotatePainter(pData);  // Rotate the painter if needed

  pShape = m_pShapeList->first();
  while( pShape )
  {
    pShapeData = pShape->shapeData();

    switch( pShapeData->shapeType() )
    {
      case KivioShapeData::kstArc:
        drawArc( pShape, pData );
        break;

      case KivioShapeData::kstPie:
        drawPie( pShape, pData );
        break;

      case KivioShapeData::kstLineArray:
        drawLineArray( pShape, pData );
        break;

      case KivioShapeData::kstPolyline:
        drawPolyline( pShape, pData );
        break;

      case KivioShapeData::kstPolygon:
        drawPolygon( pShape, pData );
        break;

      case KivioShapeData::kstBezier:
        drawBezier( pShape, pData );
        break;

      case KivioShapeData::kstRectangle:
        drawRectangle( pShape, pData );
        break;

      case KivioShapeData::kstRoundRectangle:
        drawRoundRectangle( pShape, pData );
        break;

      case KivioShapeData::kstEllipse:
        drawEllipse( pShape, pData );
        break;

      case KivioShapeData::kstOpenPath:
        drawOpenPath( pShape, pData );
        break;

      case KivioShapeData::kstClosedPath:
        drawClosedPath( pShape, pData );
        break;

      case KivioShapeData::kstTextBox:
        drawTextBox( pShape, pData );
        break;

      case KivioShapeData::kstNone:
      default:
        break;
    }

    pShape = m_pShapeList->next();
  }

  pData->painter->restoreState();
}


/**
 * Paints the connector targets of this stencil.
 */
void KivioSMLStencil::paintConnectorTargets( KivioIntraStencilData *pData )
{
  // We don't draw these if we are selected!!!
  //if( isSelected() == true ) {
  //  return;
  //}

  QPixmap targetPic;
  KivioPainter *painter;
  int x, y;

  // Obtain the graphic used for KivioConnectorTargets
  targetPic = Kivio::connectorTargetPixmap();


  m_zoomHandler = pData->zoomHandler;
  painter = pData->painter;

  KivioConnectorTarget *pTarget;
  pTarget = m_pConnectorTargets->first();
  while( pTarget )
  {
    x = m_zoomHandler->zoomItX(pTarget->x());
    y = m_zoomHandler->zoomItY(pTarget->y());

    painter->drawPixmap( x-3, y-3, targetPic );

    pTarget = m_pConnectorTargets->next();
  }
}


void KivioSMLStencil::drawArc( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _a, _l, _x, _y, _w, _h;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;
  KivioPoint *pPoint;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

  pPoint = pShapeData->pointList()->first();
  _a = m_zoomHandler->zoomItX(pPoint->x());
  _l = m_zoomHandler->zoomItY(pPoint->y());

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
    case KivioFillStyle::kcsNone:   // Hollow
      painter->drawArc( _x, _y, _w, _h, _a, _l );
      break;

    case KivioFillStyle::kcsSolid:  // Solid fill
    case KivioFillStyle::kcsGradient:               // Gradient
      painter->setFillStyle( pShapeData->fillStyle() );
      painter->drawArc( _x, _y, _w, _h, _a, _l );
      break;

    case KivioFillStyle::kcsPixmap:
    default:
      break;
  }
}

void KivioSMLStencil::drawBezier( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();


  painter = pData->painter;
  pShapeData = pShape->shapeData();

  KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPointArray controlPoints( 4 );


  pPoint = pPointList->first();
  pPoint2 = pPointList->next();
  pPoint3 = pPointList->next();
  pPoint4 = pPointList->next();


  controlPoints.setPoint( 0, m_zoomHandler->zoomItX((pPoint->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h));
  controlPoints.setPoint( 1, m_zoomHandler->zoomItX((pPoint2->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint2->y()/defHeight)*m_h));
  controlPoints.setPoint( 2, m_zoomHandler->zoomItX((pPoint3->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint3->y()/defHeight)*m_h));
  controlPoints.setPoint( 3, m_zoomHandler->zoomItX((pPoint4->x() / defWidth)*m_w),
    m_zoomHandler->zoomItY((pPoint4->y()/defHeight)*m_h));

  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  painter->drawBezier( controlPoints );
}

void KivioSMLStencil::drawOpenPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPoint, *pNewPoint;

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();

  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
  pNewPoints->setAutoDelete(true);

  pPoint = pPointList->first();
  while( pPoint )
  {
    pNewPoint = new KivioPoint( m_zoomHandler->zoomItX((pPoint->x()/defWidth)*m_w),
                                m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h),
                                pPoint->pointType() );
    pNewPoints->append(pNewPoint);

    pPoint = pPointList->next();
  }

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  painter->drawOpenPath( pNewPoints );

  delete pNewPoints;
}

void KivioSMLStencil::drawClosedPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
  KivioPainter *painter;
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPoint, *pNewPoint;

  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();

  QPtrList <KivioPoint> *pPointList = pShapeData->pointList();
  QPtrList <KivioPoint> *pNewPoints = new QPtrList<KivioPoint>;
  pNewPoints->setAutoDelete(true);

  pPoint = pPointList->first();
  while( pPoint )
  {
      pNewPoint = new KivioPoint( m_zoomHandler->zoomItX((pPoint->x()/defWidth)*m_w),
                                  m_zoomHandler->zoomItY((pPoint->y()/defHeight)*m_h),
                                  pPoint->pointType() );
      pNewPoints->append(pNewPoint);

      pPoint = pPointList->next();
  }

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
      case KivioFillStyle::kcsNone:   // Hollow
        painter->drawOpenPath( pNewPoints );
        break;

      case KivioFillStyle::kcsSolid:  // Solid fill
      case KivioFillStyle::kcsGradient:               // Gradient
        painter->setFillStyle( pShapeData->fillStyle() );
        painter->drawClosedPath( pNewPoints );
        break;

      case KivioFillStyle::kcsPixmap:
      default:
        break;
  }

  delete pNewPoints;
}

void KivioSMLStencil::drawPie( KivioShape *, KivioIntraStencilData * )
{
}

void KivioSMLStencil::drawEllipse( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _x, _y, _w, _h;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
    case KivioFillStyle::kcsNone:   // Hollow
      painter->drawEllipse( _x, _y, _w, _h );
      break;

    case KivioFillStyle::kcsSolid:  // Solid fill
    case KivioFillStyle::kcsGradient:               // Gradient
      painter->setFillStyle( pShapeData->fillStyle() );
      painter->fillEllipse( _x, _y, _w, _h );
      break;

    case KivioFillStyle::kcsPixmap:
    default:
      break;
  }
}

void KivioSMLStencil::drawLineArray( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _x, _y;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    arr.setPoint( i, _x, _y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  painter->drawLineArray( arr );
}

void KivioSMLStencil::drawRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _x, _y, _w, _h;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
    case KivioFillStyle::kcsNone:   // Hollow
      painter->drawRect( _x, _y, _w, _h );
      break;

    case KivioFillStyle::kcsSolid:  // Solid fill
    case KivioFillStyle::kcsGradient:               // Gradient
      painter->setFillStyle( pShapeData->fillStyle() );
      painter->fillRect( _x, _y, _w, _h );
      break;

    case KivioFillStyle::kcsPixmap:
    default:
      break;
  }
}

void KivioSMLStencil::drawRoundRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _rx, _ry, _x, _y, _w, _h;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  KivioPoint *pPosition, *pDimensions;
  KivioPoint *pPoint;

  pShapeData = pShape->shapeData();
  pPosition = pShapeData->position();
  pDimensions = pShapeData->dimensions();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  _x = m_zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  _y = m_zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  _w = m_zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  _h = m_zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;

  pPoint = pShapeData->pointList()->first();
  _rx = m_zoomHandler->zoomItX(pPoint->x());
  _ry = m_zoomHandler->zoomItY(pPoint->y());

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
    case KivioFillStyle::kcsNone:   // Hollow
      painter->drawRoundRect( _x, _y, _w, _h, _rx, _ry );
      break;

    case KivioFillStyle::kcsSolid:  // Solid fill
    case KivioFillStyle::kcsGradient:               // Gradient
      painter->setFillStyle( pShapeData->fillStyle() );
      painter->fillRoundRect( _x, _y, _w, _h, _rx, _ry );
      break;

    case KivioFillStyle::kcsPixmap:
    default:
      break;
  }
}

void KivioSMLStencil::drawPolygon( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _x, _y;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );


  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);


    arr.setPoint( i, _x, _y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  switch( pShapeData->fillStyle()->colorStyle() )
  {
    case KivioFillStyle::kcsNone:   // Hollow
      painter->drawPolygon(arr);
      break;

    case KivioFillStyle::kcsSolid:  // Solid fill
    case KivioFillStyle::kcsGradient:               // Gradient
      painter->setFillStyle( pShapeData->fillStyle() );
      painter->drawPolygon(arr);
      break;

    case KivioFillStyle::kcsPixmap:
    default:
      break;
  }

}

void KivioSMLStencil::drawPolyline( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth, defHeight;
  int _x, _y;
  KivioPainter *painter;
  KivioShapeData *pShapeData;
  QPtrList <KivioPoint> *pList;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();


  QPointArray arr( pList->count() );

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    arr.setPoint( i, _x, _y );

    i++;

    pPoint = pList->next();
  }

  painter = pData->painter;
  painter->setLineStyle(pShapeData->lineStyle());
  double lineWidth = pShapeData->lineStyle()->width();
  painter->setLineWidth(m_zoomHandler->zoomItY(lineWidth));

  painter->drawPolyline(arr);
}

void KivioSMLStencil::drawTextBox( KivioShape *pShape, KivioIntraStencilData *pData )
{
  double defWidth = m_pSpawner->defWidth();
  double defHeight = m_pSpawner->defHeight();
  KivioShapeData *pShapeData = pShape->shapeData();
  KivioPoint *pPosition = pShapeData->position();
  KivioPoint *pDimensions = pShapeData->dimensions();
  KivioPainter *painter = pData->painter;
  KoZoomHandler* zoomHandler = pData->zoomHandler;


  if(pShapeData->text().isEmpty()) {
    return;
  }

  int _x = zoomHandler->zoomItX((pPosition->x() / defWidth) * m_w);
  int _y = zoomHandler->zoomItY((pPosition->y() / defHeight) * m_h);
  int _w = zoomHandler->zoomItX((pDimensions->x() / defWidth) * m_w) + 1;
  int _h = zoomHandler->zoomItY((pDimensions->y() / defHeight) * m_h) + 1;


  QFont f = pShapeData->textFont();
  f.setPointSizeFloat(f.pointSizeFloat() * (((float)zoomHandler->zoom()) / 100.0));
  painter->setFont( f );
  painter->setTextColor( pShapeData->textColor() );

  int tf = pShapeData->vTextAlign() | pShapeData->hTextAlign();
  painter->drawText( _x, _y, _w, _h, tf | Qt::TextWordWrap, pShapeData->text() );
  // TODO Implement richtext support
}




/**
 * Set the fg color of this stencil.
 */
void KivioSMLStencil::setFGColor( QColor c )
{
    KivioShape *pShape;


    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->lineStyle()->setColor( c );

        pShape = m_pShapeList->next();
    }
}


/**
 * Set the bg color of this stencil.
 */
void KivioSMLStencil::setBGColor( QColor c )
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->fillStyle()->setColor( c );

        pShape = m_pShapeList->next();
    }
}

void KivioSMLStencil::setFillPattern(int p)
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->fillStyle()->setBrushStyle( static_cast<Qt::BrushStyle>(p) );

        pShape = m_pShapeList->next();
    }
}

/**
 * Set the text color of this stencil.
 */
void KivioSMLStencil::setTextColor( QColor c )
{
    KivioShape *pShape;


    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->setTextColor( c );

        pShape = m_pShapeList->next();
    }
}

/**
 * Set the text font of this stencil
 */
void KivioSMLStencil::setTextFont( const QFont &f )
{
    KivioShape *pShape;


    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->setTextFont( f );

        pShape = m_pShapeList->next();
    }
}


/**
 * Set the line width of this stencil.
 */
void KivioSMLStencil::setLineWidth( double f )
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->lineStyle()->setWidth( f );

        pShape = m_pShapeList->next();
    }
}

void KivioSMLStencil::setLinePattern(int p)
{
    KivioShape *pShape = m_pShapeList->first();

    while( pShape )
    {
        pShape->shapeData()->lineStyle()->setStyle( p );
        pShape = m_pShapeList->next();
    }
}

/**
 * Attempts to connect a KivioConnectorPoint to this stencil.
 *
 * This function will attempt to locate a KivioConnectorTarget in this
 * stencil with-in a given threshold.  If it finds it, it will connect
 * the point to it, and return the target of the connection.
 */
KivioConnectorTarget *KivioSMLStencil::connectToTarget( KivioConnectorPoint *p, double threshHold )
{
    double px = p->x();
    double py = p->y();

    double tx, ty;

    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    while( pTarget )
    {
        tx = pTarget->x();
        ty = pTarget->y();

        if( px >= tx - threshHold &&
            px <= tx + threshHold &&
            py >= ty - threshHold &&
            py <= ty + threshHold )
        {
            // setTarget calls pTarget->addConnectorPoint() and removes
            // any previous connections from p
            p->setTarget( pTarget );
            return pTarget;
        }

        pTarget = m_pConnectorTargets->next();
    }

    return NULL;
}

KoPoint KivioSMLStencil::snapToTarget( const KoPoint& p, double thresh, bool& hit )
{
    KoPoint retVal = p;
    double tx, ty;
    hit = false;

    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    while( pTarget )
    {
        tx = pTarget->x();
        ty = pTarget->y();

        if( retVal.x() >= tx - thresh &&
            retVal.x() <= tx + thresh &&
            retVal.y() >= ty - thresh &&
            retVal.y() <= ty + thresh )
        {
            retVal.setX(tx);
            retVal.setY(ty);
            hit = true;
        }

        pTarget = m_pConnectorTargets->next();
    }

    return retVal;
}

/**
 * Connects a KivioConnectorPoint to this stencil via targetID.
 *
 * This function is called during loads, *ONLY* loads.
 */
KivioConnectorTarget *KivioSMLStencil::connectToTarget( KivioConnectorPoint *p, int /*targetID*/ )
{
    int id = p->targetId();

    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        if( pTarget->id() == id )
        {
            p->setTarget(pTarget);

            return pTarget;
        }

        pTarget = m_pConnectorTargets->next();
    }

    return NULL;
}


/**
 * Updates the geometry of this stencil.
 *
 * This function rearranges the KivioConnectorTargets to reflect
 * that of the width/height/position.
 */
void KivioSMLStencil::updateGeometry()
{
  //kDebug(43000) << "m_x = " << m_x << " m_y = " << m_y << endl;

  QMatrix m;
  m.translate(m_x, m_y);
  m.translate(m_w / 2.0, m_h / 2.0);
  m.rotate(m_rotation);
  m.translate(-m_w / 2.0, -m_h / 2.0);

  KivioConnectorTarget* pTarget = m_pConnectorTargets->first();

  while( pTarget )
  {
    double _x = pTarget->xOffset() * m_w;
    double _y = pTarget->yOffset() * m_h;
    double newX = _x * m.m11() + _y * m.m21() + m.dx();
    double newY = _x * m.m12() + _y * m.m22() + m.dy();
    pTarget->setPosition( newX, newY );

    pTarget = m_pConnectorTargets->next();
  }
}


/**
 * Gets the font of this stencil
 */
QFont KivioSMLStencil::textFont()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
        {
            return pShape->shapeData()->textFont();
        }

        pShape = m_pShapeList->next();
    }

    //return QFont("times",12);
    return KoGlobal::defaultFont();
}

QColor KivioSMLStencil::textColor()
{
  KivioShape *pShape;

  pShape = m_pShapeList->first();
  while( pShape )
  {
    if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
    {
      return pShape->shapeData()->textColor();
    }

    pShape = m_pShapeList->next();
  }

  return QColor(0, 0, 0);
}

/**
 * Sets the horizontal alignemnt of this stencil.
 */
void KivioSMLStencil::setHTextAlign(int i)
{
    KivioShape *pShape;


    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
            pShape->shapeData()->setHTextAlign( i );

        pShape = m_pShapeList->next();
    }
}


/**
 * Set the vertical alignment of this stencil
 */
void KivioSMLStencil::setVTextAlign(int i)
{
    KivioShape *pShape;


    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
            pShape->shapeData()->setVTextAlign( i );

        pShape = m_pShapeList->next();
    }
}


/**
 * Get the horizontal alignment of this stencil.
 */
int KivioSMLStencil::hTextAlign()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
        {
            return pShape->shapeData()->hTextAlign();
        }

        pShape = m_pShapeList->next();
    }

    return 1;
}


/**
 * Get the vertical text alignment of this stencil.
 */
int KivioSMLStencil::vTextAlign()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
        {
            return pShape->shapeData()->vTextAlign();
        }

        pShape = m_pShapeList->next();
    }

    return 1;
}


/**
 * Get the text of this stencil
 */
QString KivioSMLStencil::text()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
        {
            return pShape->shapeData()->text();
        }

        pShape = m_pShapeList->next();
    }

    return QString("");
}


/**
 * Set the text of this stencil
 */
void KivioSMLStencil::setText( const QString &t )
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        if( pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox )
        {
            pShape->shapeData()->setText(t);
        }

        pShape = m_pShapeList->next();
    }
}


/**
 * Get the line width of this stencil
 */
double KivioSMLStencil::lineWidth()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    if( pShape )
        return pShape->shapeData()->lineStyle()->width();

    return 1.0f;
}

int KivioSMLStencil::linePattern()
{
    KivioShape *pShape = m_pShapeList->first();

    if( pShape )
        return pShape->shapeData()->lineStyle()->style();

    return 1;
}

/**
 * Get the Fg color of a stencil
 */
QColor KivioSMLStencil::fgColor()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    if( pShape )
        return pShape->shapeData()->lineStyle()->color();

    return QColor(0,0,0);
}


/**
 * Get the bg color of this stencil.
 */
QColor KivioSMLStencil::bgColor()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    if( pShape )
        return pShape->shapeData()->fillStyle()->color();

    return QColor(0,0,0);
}

int KivioSMLStencil::fillPattern()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    if( pShape )
        return pShape->shapeData()->fillStyle()->brushStyle();

    return 1;
}

/**
 * Generates the ids for anything needed by this stencil
 */
int KivioSMLStencil::generateIds( int nextAvailable )
{
    KivioConnectorTarget *pTarget = m_pConnectorTargets->first();

    // Iterate through all the targets
    while( pTarget )
    {
        // If this target has something connected to it
        if( pTarget->hasConnections() )
        {
            // Set it's id to the next available id
            pTarget->setId( nextAvailable );

            // Increment the next available id
            nextAvailable++;
        }
        else
        {
            // Otherwise mark it as unused (-1)
            pTarget->setId( -1 );
        }

        pTarget = m_pConnectorTargets->next();
    }

    // Return the next availabe id
    return nextAvailable;
}

/**
 * Check for a collision in this stencil.
 */
KivioCollisionType KivioSMLStencil::checkForCollision( KoPoint *pPoint, double )
{
  KivioCollisionType type = kctNone;

  QMatrix m;
  m.translate(m_x, m_y);
  m.translate(m_w / 2.0, m_h / 2.0);
  m.rotate(m_rotation);
  m.translate(-m_w / 2.0, -m_h / 2.0);

  KoPoint pPoints[4];
  pPoints[0].setX(0 * m.m11() + 0 * m.m21() + m.dx());
  pPoints[0].setY(0 * m.m12() + 0 * m.m22() + m.dy());
  pPoints[1].setX(m_w * m.m11() + 0 * m.m21() + m.dx());
  pPoints[1].setY(m_w * m.m12() + 0 * m.m22() + m.dy());
  pPoints[2].setX(m_w * m.m11() + m_h * m.m21() + m.dx());
  pPoints[2].setY(m_w * m.m12() + m_h * m.m22() + m.dy());
  pPoints[3].setX(0 * m.m11() + m_h * m.m21() + m.dx());
  pPoints[3].setY(0 * m.m12() + m_h * m.m22() + m.dy());

  if(PointInPoly(pPoints, 4, pPoint)) {
    type = kctBody;
  }

  return type;
}

bool KivioSMLStencil::checkCollisionArc( KivioShape *, KoPoint * )
{
    return false;
}


bool KivioSMLStencil::checkCollisionBezier( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionOpenPath( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionClosedPath( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionPie( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionEllipse( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionLineArray( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionRectangle( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionRoundRectangle( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionPolygon( KivioShape *pShape, KoPoint *pCheckPoint )
{
  double _x, _y, defWidth, defHeight;
  KivioShapeData *pShapeData;
  QPtrList<KivioPoint> *pList;
  KoPoint *pPoints;

  pShapeData = pShape->shapeData();

  defWidth = m_pSpawner->defWidth();
  defHeight = m_pSpawner->defHeight();

  pList = pShapeData->pointList();

  pPoints = new KoPoint[pList->count()];

  KivioPoint *pPoint;
  int i=0;
  pPoint = pList->first();
  while( pPoint )
  {
    _x = m_zoomHandler->zoomItX((pPoint->x() / defWidth) * m_w);
    _y = m_zoomHandler->zoomItY((pPoint->y() / defHeight) * m_h);

    pPoints[i].setX(_x);
    pPoints[i].setY(_y);

    i++;

    pPoint = pList->next();
  }

  if( PointInPoly( pPoints, i, pCheckPoint ) )
  {
    delete [] pPoints;
    return true;
  }

  delete [] pPoints;

  return false;
}

bool KivioSMLStencil::checkCollisionPolyline( KivioShape *, KoPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionTextBox( KivioShape *, KoPoint * )
{
    return false;
}


/**
 * Return a set of bits representing what resize handles are available.
 */
int KivioSMLStencil::resizeHandlePositions()
{
   // Calculate the resize handle positions
   int mask = KIVIO_RESIZE_HANDLE_POSITION_ALL;

   if( m_pProtection->at( kpWidth ) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpE | krhpW);
   }

   if( m_pProtection->at( kpHeight ) )
   {
      mask &= ~(krhpNE | krhpNW | krhpSW | krhpSE | krhpN | krhpS);
   }

   if( m_pProtection->at( kpAspect ) )
   {
      mask = KIVIO_RESIZE_HANDLE_POSITION_CORNERS;
   }

   return mask;
}

KivioLineStyle KivioSMLStencil::lineStyle()
{
    KivioShape *pShape = m_pShapeList->first();

    if( pShape )
        return *(pShape->shapeData()->lineStyle());

    return KivioLineStyle();
}

void KivioSMLStencil::setLineStyle(KivioLineStyle ls)
{
    KivioShape *pShape = m_pShapeList->first();

    while( pShape )
    {
        pShape->shapeData()->setLineStyle(ls);;
        pShape = m_pShapeList->next();
    }
}

QString KivioSMLStencil::getTextBoxName(const KoPoint& p)
{
  KivioShape* pShape = m_pShapeList->first();
  KivioShape* first = 0;
  KoPoint pos = p;
  
  // Calculate the rotation...
  QMatrix m;
  m.translate(m_x, m_y);
  m.translate(m_w / 2.0, m_h / 2.0);
  m.rotate(m_rotation);
  m.translate(-m_w / 2.0, -m_h / 2.0);
  
  while(pShape)
  {
    if(pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox)
    {
      double x = pShape->shapeData()->x();
      double y = pShape->shapeData()->y();
      double x2 = pShape->shapeData()->w() + x;
      double y2 = pShape->shapeData()->h() + y;
      
      // Create the rotated rectangle
      KoPoint pPoints[4];
      pPoints[0].setX(x * m.m11() + y * m.m21() + m.dx());
      pPoints[0].setY(x * m.m12() + y * m.m22() + m.dy());
      pPoints[1].setX(x2 * m.m11() + y * m.m21() + m.dx());
      pPoints[1].setY(x2 * m.m12() + y * m.m22() + m.dy());
      pPoints[2].setX(x2 * m.m11() + y2 * m.m21() + m.dx());
      pPoints[2].setY(x2 * m.m12() + y2 * m.m22() + m.dy());
      pPoints[3].setX(x * m.m11() + y2 * m.m21() + m.dx());
      pPoints[3].setY(x * m.m12() + y2 * m.m22() + m.dy());
    
      if(PointInPoly(pPoints, 4, &pos)) {
        return pShape->shapeData()->name();
      }
      
      if(!first) {
        first = pShape;
      }
    }

    pShape = m_pShapeList->next();
  }
  
  if(first) {
    return first->shapeData()->name();
  }
  
  return QString::null;
}

void KivioSMLStencil::setText(const QString& text, const QString& name)
{
  KivioShape* pShape = m_pShapeList->first();
  
  while(pShape)
  {
    if(pShape->shapeData()->name() == name)
    {
      pShape->shapeData()->setText(text);
      return;
    }

    pShape = m_pShapeList->next();
  }
}

QString KivioSMLStencil::text(const QString& name)
{
  KivioShape* pShape = m_pShapeList->first();
  
  while(pShape)
  {
    if(pShape->shapeData()->name() == name)
    {
      return pShape->shapeData()->text();
    }

    pShape = m_pShapeList->next();
  }
  
  return QString::null;
}

void KivioSMLStencil::addConnectorTarget(const KoPoint& p)
{
  KivioConnectorTarget* target = new KivioConnectorTarget(p.x(), p.y(), (p.x() - x())/ w(), (p.y() - y())/ h());
  m_pConnectorTargets->append(target);
}

void KivioSMLStencil::removeConnectorTarget(const KoPoint& pos)
{
  KivioConnectorTarget* target = m_pConnectorTargets->first();

  while(target)
  {
    if(target->position() == pos)
    {
      m_pConnectorTargets->remove(target);
      return;
    }

    target = m_pConnectorTargets->next();
  }
}

bool KivioSMLStencil::hasTextBox() const
{
  KivioShape* pShape = m_pShapeList->first();
  while(pShape) {
    if(pShape->shapeData()->shapeType() == KivioShapeData::kstTextBox) {
      return true;
    }

    pShape = m_pShapeList->next();
  }

  return false;
}

QColor KivioSMLStencil::textColor(const QString& textBoxName)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape)
    return textColor();

  return shape->shapeData()->textColor();
}

void KivioSMLStencil::setTextColor(const QString& textBoxName, const QColor& color)
{
  KivioShape* shape = locateShape(textBoxName);

  // If we didn't find the shape set the color stencil wide
  if(!shape) {
    setTextColor(color);
    return;
  }

  shape->shapeData()->setTextColor(color);
}

QFont KivioSMLStencil::textFont(const QString& textBoxName)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape)
    return textFont();

  return shape->shapeData()->textFont();
}

void KivioSMLStencil::setTextFont(const QString& textBoxName, const QFont& font)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape) {
    setTextFont(font);
    return;
  }

  shape->shapeData()->setTextFont(font);
}

int KivioSMLStencil::hTextAlign(const QString& textBoxName)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape)
    return hTextAlign();

  return shape->shapeData()->hTextAlign();
}

int KivioSMLStencil::vTextAlign(const QString& textBoxName)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape)
    return vTextAlign();

  return shape->shapeData()->vTextAlign();
}

void KivioSMLStencil::setHTextAlign(const QString& textBoxName, int align)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape) {
    setHTextAlign(align);
    return;
  }

  shape->shapeData()->setHTextAlign(align);
}

void KivioSMLStencil::setVTextAlign(const QString& textBoxName, int align)
{
  KivioShape* shape = locateShape(textBoxName);

  if(!shape) {
    setVTextAlign(align);
    return;
  }

  shape->shapeData()->setVTextAlign(align);
}
