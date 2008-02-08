/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
   Copyright (C) 2001 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You shouldlemente received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TableRowElement.h"
#include "TableElement.h"
#include "TableEntryElement.h"
#include "AttributeManager.h"
#include <KoXmlReader.h>
#include <QStringList>

TableRowElement::TableRowElement( BasicElement* parent ) : BasicElement( parent )
{}

TableRowElement::~TableRowElement()
{}

void TableRowElement::paint( QPainter& painter, AttributeManager* am )
{
    // nothing to paint
    Q_UNUSED( painter )
    Q_UNUSED( am )
}

void TableRowElement::layout( const AttributeManager* am )
{
    // Get the parent table to query width/ height values
    TableElement* parentTable = static_cast<TableElement*>( parentElement() );
    setHeight( parentTable->rowHeight( this ) );

    // Get alignment for every table entry
    QList<Align> verticalAlign = alignments( Qt::Vertical );
    QList<Align> horizontalAlign = alignments( Qt::Horizontal );

    // align the row's entries
    QPointF origin;
    double hOffset = 0.0;
    for ( int i = 0; i < m_entries.count(); i++ ) {
        origin = QPointF();
        hOffset = 0.0;
        if( verticalAlign[ i ] == Bottom )
            origin.setY( height() - m_entries[ i ]->height() );
        else if( verticalAlign[ i ] == Center || verticalAlign[ i ] == BaseLine )
            origin.setY( ( height() - m_entries[ i ]->height() ) / 2 );
            // Baseline is treated like Center for the moment until someone also refines
            // TableElement::determineDimensions so that it pays attention to baseline.
            // Axis as alignment option is ignored as it is tought to be an option for
            // the table itsself.
     
        if( horizontalAlign[ i ] == Center )
            hOffset = ( parentTable->columnWidth( i ) - m_entries[ i ]->width() ) / 2;
        else if( horizontalAlign[ i ] == Right )
            hOffset = parentTable->columnWidth( i ) - m_entries[ i ]->width();

        m_entries[ i ]->setOrigin( origin + QPointF( hOffset, 0.0 ) );
        origin += QPointF( parentTable->columnWidth( i ), 0.0 );
    }

    setWidth( origin.x() );
    // setting of the baseline should not be needed as the table row will only occur
    // inside a table where it does not matter if a table row has a baseline or not
}

BasicElement* TableRowElement::acceptCursor( const FormulaCursor* cursor )
{
    return 0;
}

void TableRowElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
}

void TableRowElement::removeChild( BasicElement* element )
{
}

const QList<BasicElement*> TableRowElement::childElements()
{
    QList<BasicElement*> tmp;
    foreach( TableEntryElement* element, m_entries )
        tmp << element;

    return tmp;
}

QList<Align> TableRowElement::alignments( Qt::Orientation orientation )
{
    // choose name of the attribute to query
    QString align = ( orientation == Qt::Horizontal ) ? "columnalign" : "rowalign";

    // get the alignment values of the parental TableElement
    AttributeManager am;
    QList<Align> parentAlignList = am.alignListOf( align, this );

    // iterate over all entries and look on per entry specification of alignment
    QList<Align> alignList;
    for( int i = 0; i < m_entries.count(); i++ ) {
        // element got own value for align
        if( !m_entries[ i ]->attribute( align ).isEmpty() )
            alignList << am.alignOf( align, m_entries[ i ] );
        else if( i < parentAlignList.count() )
            alignList << parentAlignList[ i ];
        else
            alignList << parentAlignList.last();
    }
    return alignList;
}

bool TableRowElement::readMathMLContent( const KoXmlElement& element )
{
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, element )
    {
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
	if( tmpElement->elementType() != TableEntry )
            return false;

        m_entries << static_cast<TableEntryElement*>( tmpElement );
	tmpElement->readMathML( tmp );
    }

    return true;
}

void TableRowElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    foreach( TableEntryElement* tmpEntry, m_entries )
        tmpEntry->writeMathML( writer );
}

ElementType TableRowElement::elementType() const
{
    return TableRow;
}

