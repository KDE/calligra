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

#include "MatrixElement.h"
#include "MatrixRowElement.h"
#include "MatrixEntryElement.h"
#include <KoXmlReader.h>
#include <QPainter>

MatrixElement::MatrixElement( BasicElement* parent ) : BasicElement( parent )
{
}

MatrixElement::~MatrixElement()
{
}

void MatrixElement::paint( QPainter& painter ) const
{
    // TODO paint the frame, rowlines, columnlines
    // eventually paint the frame
}

void MatrixElement::layout( const AttributeManager* am )
{
    // TODO implement rowspacing
    double tmpHeight = 0.0;
    double tmpWidth = 0.0;
    QPointF tmpOrigin = origin();
    foreach( MatrixRowElement* tmpRow, m_matrixRowElements )
    {
        tmpRow->layout( am );
        tmpWidth = qMax( tmpRow->width(), tmpWidth );
	tmpHeight += tmpRow->height();
	tmpRow->setOrigin( tmpOrigin );
	tmpOrigin = origin() + QPointF( 0, tmpHeight ); 
    }
    setHeight( tmpHeight );
    setWidth( tmpWidth );
    setBaseLine( tmpHeight/2 + parentElement()->height()/2 );
}

const QList<BasicElement*> MatrixElement::childElements()
{
    QList<BasicElement*> tmp;
    foreach( MatrixRowElement* tmpRow, m_matrixRowElements )
        tmp << tmpRow;
    return tmp;
}

BasicElement* MatrixElement::acceptCursor( CursorDirection direction )
{
    return 0;
}

QString MatrixElement::attributesDefaultValue( const QString& attribute ) const
{
/*    if( attribute == "align" )
        return "axis";
    else if( attribute == "rowalign" )
        return "baseline";
    else if( attribute == "columnalign" )
        return "center";
    else if( attribute == "groupalign" )
        return "{left}";
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
        return "0.8em";*/
    return QString();
}

bool MatrixElement::readMathMLContent( const KoXmlElement& element )
{  
    MatrixRowElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, element )   // iterate over the elements
    {
        tmpElement = new MatrixRowElement( this );
        m_matrixRowElements << tmpElement;
	tmpElement->readMathML( tmp );
    }

    return true;
}

void MatrixElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    foreach( MatrixRowElement* tmpRow, m_matrixRowElements )  // write each mtr element
	tmpRow->writeMathML( writer );
}

int MatrixElement::indexOfRow( BasicElement* row ) const
{
    for( int i = 0; i < m_matrixRowElements.count(); i++ )
        if( m_matrixRowElements[ i ] == row )
            return i;
    return 0;
}
/*
int MatrixElement::rows() const
{
    return m_matrixRowElements.count();
}

int MatrixElement::cols() const
{
    return m_matrixRowElements[ 0 ]->childElements().count();
}

MatrixEntryElement* MatrixElement::matrixEntryAt( int row, int col )
{
    return m_matrixRowElements[ row ]->entryAtPosition( col );
}
*/

