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
#include "kivio_common.h"
#include "kivio_config.h"
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
#include "kivio_sml_stencil_spawner.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"


#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <kdebug.h>
#include <math.h>

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
    m_pShapeList = new QList<KivioShape>;
    m_pShapeList->setAutoDelete(true);

    m_pConnectorTargets = new QList<KivioConnectorTarget>;
    m_pConnectorTargets->setAutoDelete(true);

//    m_pOriginalConnectorTargets = new QList<KivioConnectorTarget>;
//    m_pOriginalConnectorTargets->setAutoDelete(true);
}


/**
 * Destructor
 *
 * Deletes the allocated objects.
 */
KivioSMLStencil::~KivioSMLStencil()
{
    if( m_pShapeList )
    {
        delete m_pShapeList;
        m_pShapeList = NULL;
    }

    if( m_pConnectorTargets )
    {
        delete m_pConnectorTargets;
        m_pConnectorTargets = NULL;
    }
    
//    if( m_pOriginalConnectorTargets )
//    {
//        delete m_pOriginalConnectorTargets;
//        m_pOriginalConnectorTargets = NULL;
//    }

    m_pSubSelection = NULL;
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
            pShape->loadXML( ele );
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
        }

        pTarget = m_pConnectorTargets->next();    
        node = node.nextSibling();
    }
}


/**
 * Locates a shape in the shape list by name.
 */
KivioShape *KivioSMLStencil::locateShape( const QString &name )
{
    KivioShape *pShape;

    if( name == "" )
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

    XmlWriteString( e, "title", m_pSpawner->info()->title() );
    XmlWriteString( e, "setName", m_pSpawner->set()->name() );


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
//    KivioConnectorTarget *pOriginal = pOriginalConnectorTargets->first();
    while( pTarget ) //&& pOriginal )
    {
        pNewStencil->m_pConnectorTargets->append( pTarget->duplicate() );

//        pNewStencil->m_pOriginalConnectorTargets->append( pOriginal->duplicate() );

        pTarget = m_pConnectorTargets->next();
//        pOriginal = m_pOriginalConnectorTargets->next();
    }

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

    _scale = pData->scale;
    _xoff = m_x * _scale;
    _yoff = m_y * _scale;

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
	       kdDebug() << "*** KivioShape::Paint AHHHHH!!! NO SHAPE!";
                break;
        }

        pShape = m_pShapeList->next();
    }

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
    float _a, _l, _x, _y, _w, _h, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;
    KivioPoint *pPoint;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;

    pPoint = pShapeData->pointList()->first();
    _a = pPoint->x() * _scale;
    _l = pPoint->y() * _scale;


    painter = pData->painter;
    painter->drawArc( _x, _y, _w, _h, _a, _l );
}

void KivioSMLStencil::drawOutlineBezier( KivioShape *pShape, KivioIntraStencilData *pData )
{
    KivioPainter *painter;
    KivioShapeData *pShapeData = pShape->shapeData();

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();


	painter = pData->painter;
	pShapeData = pShape->shapeData();

	KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
	QList <KivioPoint> *pPointList = pShapeData->pointList();
	QPointArray controlPoints( 4 );


	pPoint = pPointList->first();
	pPoint2 = pPointList->next();
	pPoint3 = pPointList->next();
	pPoint4 = pPointList->next();


    controlPoints.setPoint( 0, _xoff + ((pPoint->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 1, _xoff + ((pPoint2->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint2->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 2, _xoff + ((pPoint3->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint3->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 3, _xoff + ((pPoint4->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint4->y()/defHeight)*m_h)*_scale );

    painter = pData->painter;
    painter->drawBezier( controlPoints );
}

void KivioSMLStencil::drawOutlineOpenPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
    KivioPainter *painter;
    KivioShapeData *pShapeData = pShape->shapeData();
    KivioPoint *pPoint, *pNewPoint;

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();

    QList <KivioPoint> *pPointList = pShapeData->pointList();
    QList <KivioPoint> *pNewPoints = new QList<KivioPoint>;
    pNewPoints->setAutoDelete(true);

    pPoint = pPointList->first();
    while( pPoint )
    {
        pNewPoint = new KivioPoint( _xoff + ((pPoint->x()/defWidth)*m_w)*_scale,
                                    _yoff + ((pPoint->y()/defHeight)*m_h)*_scale,
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

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();

    QList <KivioPoint> *pPointList = pShapeData->pointList();
    QList <KivioPoint> *pNewPoints = new QList<KivioPoint>;
    pNewPoints->setAutoDelete(true);

    pPoint = pPointList->first();
    while( pPoint )
    {
        pNewPoint = new KivioPoint( _xoff + ((pPoint->x()/defWidth)*m_w)*_scale,
                                    _yoff + ((pPoint->y()/defHeight)*m_h)*_scale,
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
    float _x, _y, _w, _h, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    painter = pData->painter;
    painter->setFGColor( QColor(0, 0, 0) );
    painter->drawEllipse( _x, _y, _w, _h );

}

void KivioSMLStencil::drawOutlineLineArray( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    painter->drawLineArray( arr );
}

void KivioSMLStencil::drawOutlineRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, _w, _h, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    painter = pData->painter;
    painter->setFGColor( QColor(0, 0, 0) );
    painter->drawRect( _x, _y, _w, _h );

}

void KivioSMLStencil::drawOutlineRoundRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _rx, _ry, _x, _y, _w, _h, defWidth, defHeight;
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
    _rx = pPoint->x() * _scale;
    _ry = pPoint->y() * _scale;


    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    painter = pData->painter;
    painter->setFGColor( QColor(0, 0, 0) );
    painter->drawRoundRect( _x, _y, _w, _h, _rx, _ry );
}

void KivioSMLStencil::drawOutlinePie( KivioShape *, KivioIntraStencilData * )
{

}

void KivioSMLStencil::drawOutlinePolygon( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    painter->drawPolyline( arr );
}

void KivioSMLStencil::drawOutlinePolyline( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    painter->drawPolyline( arr );
}

void KivioSMLStencil::drawOutlineTextBox( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();
    float _x, _y, _w, _h;
    KivioShapeData *pShapeData = pShape->shapeData();
    KivioPoint *pPosition = pShapeData->position();
    KivioPoint *pDimensions = pShapeData->dimensions();
    KivioPainter *painter = pData->painter;


    if( pShapeData->text().length() <= 0 )
        return;


    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    QFont f = pShapeData->textFont();
    f.setPointSize( f.pointSize() * _scale );
    painter->setFont( f );
    painter->setTextColor( QColor(0, 0, 0) );


    int tf = pShapeData->vTextAlign() | pShapeData->hTextAlign();
    painter->drawText( _x, _y, _w, _h, tf | Qt::WordBreak, pShapeData->text() );
}


/**
 * Paints the stencil
 */
void KivioSMLStencil::paint( KivioIntraStencilData *pData )
{
    KivioShape *pShape;
    KivioShapeData *pShapeData;


    _scale = pData->scale;
    _xoff = m_x * _scale;
    _yoff = m_y * _scale;

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

}


/**
 * Paints the connector targets of this stencil.
 */
void KivioSMLStencil::paintConnectorTargets( KivioIntraStencilData *pData )
{
    QPixmap *targetPic;
    KivioPainter *painter;
    float x, y;
    
    // We don't draw these if we are selected!!!
    if( isSelected() == true )
      return;
    
    // Obtain the graphic used for KivioConnectorTargets
    targetPic = KivioConfig::config()->connectorTargetPixmap();
    

    _scale = pData->scale;
    painter = pData->painter;

    KivioConnectorTarget *pTarget;
    pTarget = m_pConnectorTargets->first();
    while( pTarget )
    {
        x = pTarget->x() * _scale;
        y = pTarget->y() * _scale;
        
        painter->drawPixmap( x-3, y-3, *targetPic );

        pTarget = m_pConnectorTargets->next();
    }
}


void KivioSMLStencil::drawArc( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _a, _l, _x, _y, _w, _h, defWidth, defHeight;
    float lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;
    KivioPoint *pPoint;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;

    pPoint = pShapeData->pointList()->first();
    _a = pPoint->x() * _scale;
    _l = pPoint->y() * _scale;

    lineWidth = pShapeData->lineStyle()->width();

    painter = pData->painter;

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawArc( _x, _y, _w, _h, _a, _l );
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->drawArc( _x, _y, _w, _h, _a, _l );
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
	   kdDebug() << "KivioSMLStenciL::drawArc() - gradient fill unimplemented";
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

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();


	painter = pData->painter;
	pShapeData = pShape->shapeData();

	KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
	QList <KivioPoint> *pPointList = pShapeData->pointList();
	QPointArray controlPoints( 4 );


	pPoint = pPointList->first();
	pPoint2 = pPointList->next();
	pPoint3 = pPointList->next();
	pPoint4 = pPointList->next();


    controlPoints.setPoint( 0, _xoff + ((pPoint->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 1, _xoff + ((pPoint2->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint2->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 2, _xoff + ((pPoint3->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint3->y()/defHeight)*m_h)*_scale );
    controlPoints.setPoint( 3, _xoff + ((pPoint4->x() / defWidth)*m_w)*_scale, _yoff + ((pPoint4->y()/defHeight)*m_h)*_scale );

    painter = pData->painter;
    float lineWidth = pShapeData->lineStyle()->width();
    painter->setLineWidth( (lineWidth)*_scale );
    painter->setFGColor( pShapeData->lineStyle()->color() );

    painter->drawBezier( controlPoints );
}

void KivioSMLStencil::drawOpenPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
    KivioPainter *painter;
    KivioShapeData *pShapeData = pShape->shapeData();
    KivioPoint *pPoint, *pNewPoint;

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();

    QList <KivioPoint> *pPointList = pShapeData->pointList();
    QList <KivioPoint> *pNewPoints = new QList<KivioPoint>;
    pNewPoints->setAutoDelete(true);

    pPoint = pPointList->first();
    while( pPoint )
    {
        pNewPoint = new KivioPoint( _xoff + ((pPoint->x()/defWidth)*m_w)*_scale,
                                    _yoff + ((pPoint->y()/defHeight)*m_h)*_scale,
                                    pPoint->pointType() );
        pNewPoints->append(pNewPoint);

        pPoint = pPointList->next();
    }

    painter = pData->painter;
    float lineWidth = pShapeData->lineStyle()->width();
    painter->setLineWidth( (lineWidth)*_scale );
    painter->setFGColor( pShapeData->lineStyle()->color() );

    painter->drawOpenPath( pNewPoints );

    delete pNewPoints;
}

void KivioSMLStencil::drawClosedPath( KivioShape *pShape, KivioIntraStencilData *pData )
{
    KivioPainter *painter;
    KivioShapeData *pShapeData = pShape->shapeData();
    KivioPoint *pPoint, *pNewPoint;
    float lineWidth;

    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();

    QList <KivioPoint> *pPointList = pShapeData->pointList();
    QList <KivioPoint> *pNewPoints = new QList<KivioPoint>;
    pNewPoints->setAutoDelete(true);

    pPoint = pPointList->first();
    while( pPoint )
    {
        pNewPoint = new KivioPoint( _xoff + ((pPoint->x()/defWidth)*m_w)*_scale,
                                    _yoff + ((pPoint->y()/defHeight)*m_h)*_scale,
                                    pPoint->pointType() );
        pNewPoints->append(pNewPoint);

        pPoint = pPointList->next();
    }

    painter = pData->painter;
    lineWidth = pShapeData->lineStyle()->width();

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawOpenPath( pNewPoints );
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->drawClosedPath( pNewPoints );
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
	   kdDebug() << "KivioSMLStencil::drawClosedPath() - gradient fill unimplemented";
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
    float _x, _y, _w, _h, defWidth, defHeight, lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    lineWidth = pShapeData->lineStyle()->width();
    painter = pData->painter;

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawEllipse( _x, _y, _w, _h );
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->fillEllipse( _x, _y, _w, _h );
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
	   kdDebug() << "KivioSMLStencil::drawEllipse() - gradient fill unimplemented";
            break;

        case KivioFillStyle::kcsPixmap:
        default:
            break;
    }

}

void KivioSMLStencil::drawLineArray( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    float lineWidth = pShapeData->lineStyle()->width();
    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    painter->drawLineArray( arr );
}

void KivioSMLStencil::drawRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, _w, _h, defWidth, defHeight;
    float lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    lineWidth = pShapeData->lineStyle()->width();

    painter = pData->painter;

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawRect( _x, _y, _w, _h );
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->fillRect( _x, _y, _w, _h );
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
            break;

        case KivioFillStyle::kcsPixmap:
        default:
            break;
    }
}

void KivioSMLStencil::drawRoundRectangle( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _rx, _ry, _x, _y, _w, _h, defWidth, defHeight;
    float lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    KivioPoint *pPosition, *pDimensions;
    KivioPoint *pPoint;

    pShapeData = pShape->shapeData();
    pPosition = pShapeData->position();
    pDimensions = pShapeData->dimensions();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;

    pPoint = pShapeData->pointList()->first();
    _rx = pPoint->x() * _scale;
    _ry = pPoint->y() * _scale;

    lineWidth = pShapeData->lineStyle()->width();

    painter = pData->painter;

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawRoundRect( _x, _y, _w, _h, _rx, _ry );
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->fillRoundRect( _x, _y, _w, _h, _rx, _ry );
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
	   kdDebug() << "KivioSMLStenciL::drawRoundRectangle() - gradient fill unimplemented";
            break;

        case KivioFillStyle::kcsPixmap:
        default:
            break;
    }
}

void KivioSMLStencil::drawPolygon( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight, lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;


        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    lineWidth = pShapeData->lineStyle()->width();

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    switch( pShapeData->fillStyle()->colorStyle() )
    {
        case KivioFillStyle::kcsNone:   // Hollow
            painter->drawPolygon(arr);
            break;

        case KivioFillStyle::kcsSolid:  // Solid fill
            painter->setBGColor( pShapeData->fillStyle()->color() );
            painter->drawPolygon(arr);
            break;

        case KivioFillStyle::kcsGradient:               // Gradient
	   kdDebug() << "KivioSMLStenciL::drawPolygon() - gradient fill unimplemented";
            break;

        case KivioFillStyle::kcsPixmap:
        default:
            break;
    }

}

void KivioSMLStencil::drawPolyline( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float _x, _y, defWidth, defHeight, lineWidth;
    KivioPainter *painter;
    KivioShapeData *pShapeData;
    QList <KivioPoint> *pList;

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
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        arr.setPoint( i, (int)_x, (int)_y );

        i++;

        pPoint = pList->next();
    }

    painter = pData->painter;
    lineWidth = pShapeData->lineStyle()->width();

    painter->setFGColor( pShapeData->lineStyle()->color() );
    painter->setLineWidth( (lineWidth)*_scale );

    painter->drawPolyline(arr);
}

void KivioSMLStencil::drawTextBox( KivioShape *pShape, KivioIntraStencilData *pData )
{
    float defWidth = m_pSpawner->defWidth();
    float defHeight = m_pSpawner->defHeight();
    float _x, _y, _w, _h;
    KivioShapeData *pShapeData = pShape->shapeData();
    KivioPoint *pPosition = pShapeData->position();
    KivioPoint *pDimensions = pShapeData->dimensions();
    KivioPainter *painter = pData->painter;


    if( pShapeData->text().length() <= 0 )
        return;


    _x = (pPosition->x() / defWidth) * m_w * _scale + _xoff;
    _y = (pPosition->y() / defHeight) * m_h * _scale + _yoff;
    _w = (pDimensions->x() / defWidth) * m_w * _scale + 1;
    _h = (pDimensions->y() / defHeight) * m_h * _scale + 1;


    QFont f = pShapeData->textFont();
    f.setPointSize( f.pointSize() * _scale );
    painter->setFont( f );
    painter->setTextColor( pShapeData->textColor() );


    int tf = pShapeData->vTextAlign() | pShapeData->hTextAlign();
    painter->drawText( _x, _y, _w, _h, tf | Qt::WordBreak, pShapeData->text() );
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
void KivioSMLStencil::setLineWidth( float f )
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    while( pShape )
    {
        pShape->shapeData()->lineStyle()->setWidth( f );

        pShape = m_pShapeList->next();
    }
}


/**
 * Attempts to connect a KivioConnectorPoint to this stencil.
 *
 * This function will attempt to locate a KivioConnectorTarget in this
 * stencil with-in a given threshhold.  If it finds it, it will connect
 * the point to it, and return the target of the connection.
 */
KivioConnectorTarget *KivioSMLStencil::connectToTarget( KivioConnectorPoint *p, float threshHold )
{
    float px = p->x();
    float py = p->y();

    float tx, ty;

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


/**
 * Connects a KivioConnectorPoint to this stencil via targetID.
 *
 * This function is called during loads, *ONLY* loads.
 */
KivioConnectorTarget *KivioSMLStencil::connectToTarget( KivioConnectorPoint *p, int targetID )
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
    KivioConnectorTarget *pTarget, *pOriginal;
    float _x, _y;
    float defWidth, defHeight;

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    QList<KivioConnectorTarget> *pOriginalTargets = ((KivioSMLStencilSpawner*)m_pSpawner)->targets();
    pTarget = m_pConnectorTargets->first();
    pOriginal = pOriginalTargets->first();

    while( pTarget && pOriginal )
    {
        _x = (pOriginal->x() / defWidth) * m_w  + m_x;
        _y = (pOriginal->y() / defHeight) * m_h + m_y;

        pTarget->setPosition( _x, _y );

        pTarget = m_pConnectorTargets->next();
        pOriginal = pOriginalTargets->next();
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

    return QFont("times",12);
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
float KivioSMLStencil::lineWidth()
{
    KivioShape *pShape;

    pShape = m_pShapeList->first();
    if( pShape )
        return pShape->shapeData()->lineStyle()->width();

    return 1.0f;
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
KivioCollisionType KivioSMLStencil::checkForCollision( KivioPoint *pPoint, float )
{
/*
    float px = pPoint->x();
    float py = pPoint->y();

    if( !(px < m_x + m_w &&
         px >= m_x &&
         py < m_y + m_h &&
         py >= m_y ) )
    {
        return kctNone;
    }

    return kctBody;
*/    
    
    float px = pPoint->x();
    float py = pPoint->y();

    if( !(px < m_x + m_w &&
         px >= m_x &&
         py < m_y + m_h &&
         py >= m_y ) )
    {
        return kctNone;
    }
    
    return kctBody;
    /*

    KivioShape *pShape;
    
    pShape = m_pShapeList->last();
    while( pShape )
    {
        switch( pShape->shapeType() )
        {
            case KivioShapeData::kstArc:
                if( checkCollisionArc( pShape, pPoint ) )
                    return kctBody;
                break;

            case KivioShapeData::kstPie:
                if( checkCollisionPie( pShape, pPoint ) )
                    return kctBody;
                break;

            case KivioShapeData::kstLineArray:
                if( checkCollisionLineArray( pShape, pPoint ) )
                    return kctBody;
                break;

            case KivioShapeData::kstPolyline:
                if( checkCollisionPolyline( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstPolygon:
                if( checkCollisionPolygon( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstBezier:
                if( checkCollisionBezier( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstRectangle:
                if( checkCollisionRectangle( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstRoundRectangle:
                if( checkCollisionRoundRectangle( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstEllipse:
                if( checkCollisionEllipse( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstOpenPath:
                if( checkCollisionOpenPath( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstClosedPath:
                if( checkCollisionClosedPath( pShape, pPoint) )
                    return kctBody;
                break;

            case KivioShapeData::kstTextBox:
                if( checkCollisionTextBox( pShape, pPoint) )
                    return kctBody;
                break;


            case KivioShapeData::kstNone:
            default:
                break;
        }
    
        pShape = m_pShapeList->prev();
    }

    
    return kctNone;
    */
}

bool KivioSMLStencil::checkCollisionArc( KivioShape *, KivioPoint * )
{
    return false;
}


bool KivioSMLStencil::checkCollisionBezier( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionOpenPath( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionClosedPath( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionPie( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionEllipse( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionLineArray( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionRectangle( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionRoundRectangle( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionPolygon( KivioShape *pShape, KivioPoint *pCheckPoint )
{
    float _x, _y, defWidth, defHeight;
    KivioShapeData *pShapeData;
    QList<KivioPoint> *pList;
    KivioPoint *pPoints;
    

    pShapeData = pShape->shapeData();

    defWidth = m_pSpawner->defWidth();
    defHeight = m_pSpawner->defHeight();

    pList = pShapeData->pointList();

    pPoints = new KivioPoint[pList->count()];

    KivioPoint *pPoint;
    int i=0;
    pPoint = pList->first();
    while( pPoint )
    {
        _x = (pPoint->x() / defWidth) * m_w * _scale + _xoff;
        _y = (pPoint->y() / defHeight) * m_h * _scale + _yoff;

        pPoints[i].set(_x,_y);

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

bool KivioSMLStencil::checkCollisionPolyline( KivioShape *, KivioPoint * )
{
    return false;
}

bool KivioSMLStencil::checkCollisionTextBox( KivioShape *, KivioPoint * )
{
    return false;
}


/**
 * Return a set of bits representing what resize handles are available.
 */
int KivioSMLStencil::resizeHandlePositions()
{
    return KIVIO_RESIZE_HANDLE_POSITION_ALL;
}
