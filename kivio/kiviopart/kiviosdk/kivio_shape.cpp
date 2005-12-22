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
#include <kdebug.h>
#include "kivio_common.h"
#include "kivio_fill_style.h"
#include "kivio_line_style.h"
#include "kivio_point.h"
#include "kivio_shape.h"
#include "kivio_text_style.h"

/**
 * Default constructor
 *
 * Does nothing
 */
KivioShape::KivioShape()
{
}


/**
 * Copy constructor
 *
 * @param source The object to copy data from
 *
 * Copies all data from source
 */
KivioShape::KivioShape( const KivioShape &source )
{
    source.m_shapeData.copyInto( &m_shapeData );
}


/**
 * Destructor
 */
KivioShape::~KivioShape()
{
}


/**
 * Copy all data into pTarget
 *
 * @param pTarget The target &ref KivioShape to copy into
 *
 * This will copy all data contained in this, into pTarget.
 */
void KivioShape::copyInto( KivioShape *pTarget ) const
{
    if( !pTarget )
        return;

    m_shapeData.copyInto( &(pTarget->m_shapeData) );
}


/**
 * Load this object from an XML element
 *
 * @param e The element to load from
 * @returns true on success, false on failure.
 */
bool KivioShape::loadXML( const QDomElement &e )
{
    QDomElement ele;
    QDomNode node = e.firstChild();

    QString name = XmlReadString( e, "name", "" );
    m_shapeData.setName( name );
    m_shapeData.setShapeType( (KivioShapeData::KivioShapeType)XmlReadInt( e, "shapeType", -1 ));

    if( m_shapeData.name().isEmpty() ||
        m_shapeData.shapeType() == -1 )
    {
       kdWarning(43000) << "-LOAD KivioShape::loadXML() - Unknown shape or bad name read. Shape load aborted." << endl;
       return false;
    }

    while( !node.isNull() )
    {
        QString nodeName = node.nodeName();
        ele = node.toElement();

        if( nodeName == "KivioShapeData" )
        {
            m_shapeData.loadXML( ele );
        }

        node = node.nextSibling();
    }
    return true;
}


/**
 * Save this object to an XML element
 *
 * @param doc The document to save to
 * @returns QDomElement representing this object.
 */
QDomElement KivioShape::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioShape");

    XmlWriteString( e, "name", m_shapeData.m_name );
    XmlWriteInt( e, "shapeType", m_shapeData.m_shapeType );

    e.appendChild( m_shapeData.saveXML( doc ) );

    return e;
}


/**
 * Loads a shape of type arc
 *
 * @param e The element to load from
 * @returns A newly allocated KivioShape, or NULL on error.
 *
 * FIXME: implement this
 */
KivioShape *KivioShape::loadShapeArc( const QDomElement & )
{
    return NULL;
}


/**
 * Loads a shape of type closed-path.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeClosedPath( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstClosedPath;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The line array  is made up of pairs of points
        if( nodeName == "KivioPoint" )
        {
            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint(0.0f, 0.0f, KivioPoint::kptBezier);
            pPoint->loadXML( node.toElement() );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioFillStyle" )
        {
            pShape->m_shapeData.m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }
    
        node = node.nextSibling();
    }
        
    return pShape;
}


/**
 * Loads a shape of type bezier
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeBezier( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstBezier;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The line array  is made up of pairs of points
        if( nodeName == "KivioPoint" )
        {
            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint(0.0f, 0.0f, KivioPoint::kptBezier);
            pPoint->loadXML( node.toElement() );
            if( pPoint->pointType() != KivioPoint::kptBezier )
            {
	       kdDebug(43000) << "KivioShape::loadShapeBezier() - Non-bezier point found.  Aborting shape." << endl;
                delete pPoint;
                delete pShape;
                return NULL;
            }
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    if( pShape->m_shapeData.m_pOriginalPointList->count() != 4 )
    {
       kdDebug(43000) << "KivioShape::loadShapeBezier() - Wrong number of points loaded, should be 4, shape aborted" << endl;
        delete pShape;
        return NULL;
    }
    
    return pShape;
}


/**
 * Loads a shape of type ellipse.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeEllipse( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstEllipse;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    pShape->m_shapeData.m_position.set( XmlReadFloat( e, "x", 0.0f ), XmlReadFloat( e, "y", 0.0f ) );
    pShape->m_shapeData.m_dimensions.set( XmlReadFloat( e, "w", 0.0f ), XmlReadFloat( e, "h", 0.0f ) );
        
    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        if( nodeName == "KivioFillStyle" )
        {
            pShape->m_shapeData.m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    return pShape;
}


/**
 * Loads a shape of type line-array.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeLineArray( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    QDomElement lineElement;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstLineArray;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The line array  is made up of pairs of points
        if( nodeName == "Line" )
        {
            lineElement = node.toElement();

            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint( XmlReadFloat( lineElement, "x1", 0.0f ), XmlReadFloat( lineElement, "y1", 0.0f ) );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = new KivioPoint( XmlReadFloat( lineElement, "x2", 0.0f ), XmlReadFloat( lineElement, "y2", 0.0f ) );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }

    return pShape;
}


/**
 * Loads a shape of type open-path.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeOpenPath( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstOpenPath;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The line array  is made up of pairs of points
        if( nodeName == "KivioPoint" )
        {
            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint(0.0f, 0.0f, KivioPoint::kptBezier);
            pPoint->loadXML( node.toElement() );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
        
    return pShape;
}


/**
 * Loads a shape of type pie.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 *
 * FIXME: Implement this
 */
KivioShape *KivioShape::loadShapePie( const QDomElement & )
{
    return NULL;
}


/**
 * Loads a shape of type polygon.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapePolygon( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstPolygon;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The polygon is made up of a series of points
        if( nodeName == "KivioPoint" )
        {
            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint();
            pPoint->loadXML( node.toElement() );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioFillStyle" )
        {
            pShape->m_shapeData.m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    return pShape;
}


/**
 * Loads a shape of type polyline.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapePolyline( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstPolyline;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // The polygon is made up of a series of points
        if( nodeName == "KivioPoint" )
        {
            // Allocate a new point, load it, and store it in the list
            pPoint = new KivioPoint();
            pPoint->loadXML( node.toElement() );
            pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
            pPoint = NULL;
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    return pShape;
}


/**
 * Loads a shape of type rectangle.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeRectangle( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstRectangle;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    pShape->m_shapeData.m_position.set( XmlReadFloat( e, "x", 0.0f ), XmlReadFloat( e, "y", 0.0f ) );
    pShape->m_shapeData.m_dimensions.set( XmlReadFloat( e, "w", 72.0f ), XmlReadFloat( e, "h", 72.0f ) );
    
    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // Read the fill style
        if( nodeName == "KivioFillStyle" )
        {
            pShape->m_shapeData.m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    return pShape;
}



/**
 * Loads a shape of type round rectangle.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeRoundRectangle( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    KivioPoint *pPoint = NULL;
    QDomNode node;
    QString nodeName;
    

    // Create the new shape to load into
    pShape = new KivioShape();
    
    // Load the type, name, and lineWidth properties
    pShape->m_shapeData.m_shapeType = KivioShapeData::kstRoundRectangle;
    pShape->m_shapeData.m_name = XmlReadString( e, "name", "" );

    pShape->m_shapeData.m_position.set( XmlReadFloat( e, "x", 0.0f ), XmlReadFloat( e, "y", 0.0f ) );
    pShape->m_shapeData.m_dimensions.set( XmlReadFloat( e, "w", 72.0f ), XmlReadFloat( e, "h", 72.0f ) );
    
    // Read and store the radii of the curves
    pPoint = new KivioPoint(0.0f, 0.0f);
    pPoint->set( XmlReadFloat( e, "r1", 1.0f ), XmlReadFloat( e, "r2", 1.0f ) );
    pShape->m_shapeData.m_pOriginalPointList->append( pPoint );
    
    // Iterate through the nodes loading the various items
    node = e.firstChild();
    while( !node.isNull() )
    {
        nodeName = node.nodeName();
        
        // Read the fill style
        if( nodeName == "KivioFillStyle" )
        {
            pShape->m_shapeData.m_pFillStyle->loadXML( node.toElement() );
        }
        else if( nodeName == "KivioLineStyle" )
        {
            pShape->m_shapeData.m_pLineStyle->loadXML( node.toElement() );
        }

        node = node.nextSibling();
    }
    
    return pShape;
}


/**
 * Loads a shape of type textbox.
 *
 * @param e The element to load from.
 * @returns A newly allocated KivioShape, or NULL on error.
 */
KivioShape *KivioShape::loadShapeTextBox( const QDomElement &e )
{
    KivioShape *pShape = NULL;
    QDomNode node;
    QString nodeName;
    KivioTextStyle ts;
    QString name;


    // Create the new shape to load into
    pShape = new KivioShape();

    // Load the type and name
    pShape->m_shapeData.setShapeType(KivioShapeData::kstTextBox);
    pShape->m_shapeData.setName( XmlReadString( e, "name", "" ) );
    pShape->m_shapeData.setTextColor(XmlReadColor(e,"color",QColor(0,0,0)));

    pShape->m_shapeData.m_position.set( 
       XmlReadFloat( e, "x", 0.0f ), XmlReadFloat( e, "y", 0.0f ) );
    pShape->m_shapeData.m_dimensions.set( 
       XmlReadFloat( e, "w", 72.0f ), XmlReadFloat( e, "h", 72.0f ) );

    // Now look for the KivioTextStyle
    node = e.firstChild();
    while( !node.isNull() )
    {
       nodeName = node.nodeName();
       if( nodeName == "KivioTextStyle" )
       {
	  ts.loadXML( node.toElement() );
	  pShape->m_shapeData.setTextStyle( &ts );
       }

       node = node.nextSibling();
    }
//    QString text = XmlReadString( e, "text", "" );
//    pShape->m_shapeData.setText( text );


    return pShape;
}
