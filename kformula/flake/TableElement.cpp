/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TableElement.h"
#include "AttributeManager.h"
#include "TableRowElement.h"
#include <KoXmlReader.h>
#include <QPainter>
#include <QList>

TableElement::TableElement( BasicElement* parent ) : BasicElement( parent )
{
    m_framePenStyle = Qt::NoPen;
}

TableElement::~TableElement()
{}

void TableElement::paint( QPainter& painter, AttributeManager* am )
{
    // draw frame
    if( m_framePenStyle != Qt::NoPen ) {
        painter.setPen( QPen( m_framePenStyle ) );
        painter.drawRect( QRectF( 0.0, 0.0, width(), height() ) );
    }

    // draw rowlines
    double offset = 0.0;
    for( int i = 0; i < m_rowHeights.count(); i++ ) {
        offset += m_rowHeights[ i ];
        painter.drawLine( QPointF( 0.0, offset ), QPointF( width(), offset ) );     
    }

    // draw columnlines
    offset = 0.0;
    for( int i = 0; i < m_colWidths.count(); i++ ) {
        offset += m_colWidths[ i ];
        painter.drawLine( QPointF( offset, 0.0 ), QPointF( offset, height() ) );
    }
}

void TableElement::layout( const AttributeManager* am )
{
    // lookup attribute values needed for this table
    m_framePenStyle = am->penStyleOf( "frame", this );
    m_rowLinePenStyles = am->penStyleListOf( "rowlines", this );
    m_colLinePenStyles = am->penStyleListOf( "columnlines", this );
    QList<double> frameSpacing = am->doubleListOf( "framespacing", this );
    QList<double> rowSpacing = am->doubleListOf( "rowspacing", this );

    // layout the rows vertically
    double tmpX = frameSpacing[ 0 ];
    double tmpY = frameSpacing[ 1 ];
    for( int i = 0; i < m_rows.count(); i++ ) {
        m_rows[ i ]->setOrigin( QPointF( tmpX, tmpY ) );
        tmpY += m_rows[ i ]->height();
        tmpY += ( i < rowSpacing.count() ) ? rowSpacing[ i ] : rowSpacing.last();
    }

    // add the spacing to tmpX and tmpY
    tmpX += m_rows.first()->width();
    tmpX += frameSpacing[ 0 ];
    tmpY += frameSpacing[ 1 ];
    setWidth( tmpX );
    setHeight( tmpY );
    setBaseLine( height() / 2 );
}

void TableElement::determineDimensions()
{
    AttributeManager am;
    bool equalRows = am.boolOf( "equalrows", this );
    bool equalColumns = am.boolOf( "equalcolumns", this );

    // determine the dimensions of each row and col based on the biggest element in it
    BasicElement* entry;
    double maxWidth = 0.0;
    double maxHeight = 0.0;
    for( int row = 0; row < m_rows.count(); row++ ) {
        m_rowHeights << 0.0;
        for( int col = 0; col < m_rows.first()->childElements().count(); col++ ) {
             if( m_colWidths.count() <= col )
                 m_colWidths << 0.0;

             entry = m_rows[ row ]->childElements()[ col ];
             m_colWidths[ col ] = qMax( m_colWidths[ col ], entry->width() );
             m_rowHeights[ row ] = qMax( m_rowHeights[ row ], entry->height() );
             maxWidth = qMax( entry->width(), maxWidth );
        }
        maxHeight = qMax( m_rowHeights[ row ], maxHeight );
    }

    // treat equalcol and equalrow attributes
    if( equalRows )
        for( int i = 0; i < m_rowHeights.count(); i++ )
            m_rowHeights.replace( i, maxHeight );

    if( equalColumns )
        for( int i = 0; i < m_colWidths.count(); i++ )
            m_colWidths.replace( i, maxWidth );
}

double TableElement::columnWidth( int column )
{
    if( m_colWidths.isEmpty() )
        determineDimensions();

    return m_colWidths[ column ];
}

double TableElement::rowHeight( TableRowElement* row )
{
    if( m_rowHeights.isEmpty() )
        determineDimensions();

    return m_rowHeights[ m_rows.indexOf( row ) ];
}

const QList<BasicElement*> TableElement::childElements()
{
    QList<BasicElement*> tmp;
    foreach( TableRowElement* tmpRow, m_rows )
        tmp << tmpRow;
    return tmp;
}

BasicElement* TableElement::acceptCursor( const FormulaCursor* cursor )
{
    return 0;
}

QString TableElement::attributesDefaultValue( const QString& attribute ) const
{
    if( attribute == "align" )
        return "axis";
    else if( attribute == "rowalign" )
        return "baseline";
    else if( attribute == "columnalign" )
        return "center";
    else if( attribute == "groupalign" )
        return "left";
    else if( attribute == "alignmentscope" )
        return "true";
    else if( attribute == "columnwidth" )
        return "auto";
    else if( attribute == "width" )
        return "auto";
    else if( attribute == "rowspacing" )
        return "1.0ex";
    else if( attribute == "columnspacing" )
        return "0.8em";
    else if( attribute == "rowlines" || attribute == "columnlines" ||
             attribute == "frame" )
        return "none";
    else if( attribute == "framespacing" )
        return "0.4em 0.5ex";
    else if( attribute == "equalrows" || attribute == "equalcolumns" ||
             attribute == "displaystyle" )
        return "false";
    else if( attribute == "side" )
        return "right";
    else if( attribute == "minlabelspacing" )
        return "0.8em";
    else
        return QString();
}

bool TableElement::readMathMLContent( const KoXmlElement& element )
{  
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, element )   // iterate over the elements
    {
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( tmpElement->elementType() != TableRow )
            return false;

        m_rows << static_cast<TableRowElement*>( tmpElement );
	tmpElement->readMathML( tmp );
    }

    return true;
}

void TableElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    foreach( TableRowElement* tmpRow, m_rows )  // write each mtr element
	tmpRow->writeMathML( writer );
}

