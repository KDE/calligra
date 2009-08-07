/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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


// Local
#include "TableModel.h"

// C
#include <cmath>

// Qt
#include <QDomNode>
#include <QDomDocument>

// KDE
#include <KDebug>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>

// KChart
#include "CellRegion.h"


namespace KChart {

TableModel::TableModel( QObject *parent /* = 0 */)
    : QStandardItemModel(parent)
{
}

TableModel::~TableModel()
{
}

QHash<QString, QVector<QRect> > TableModel::cellRegion() const
{
    return QHash<QString, QVector<QRect> >();
}

bool TableModel::setCellRegion( const QString& regionName )
{
    int result = 0;

    const int size = regionName.size();
    for ( int i = 0; i < size; i++ ) {
        result += CellRegion::rangeCharToInt( regionName[i].toAscii() ) * std::pow( 10.0, ( size - i - 1 ) );
    }

    return result;
}

bool TableModel::isCellRegionValid( const QString& regionName ) const
{
    Q_UNUSED( regionName );

    return true;
}

void TableModel::loadOdf( const KoXmlElement &tableElement,
			  KoShapeLoadingContext &context )
{
    setRowCount( 0 );
    setColumnCount( 0 );
    
#ifndef KOXML_USE_QDOM
    const QDomNode &node = tableElement.asQDomNode( QDomDocument() );
#else
    const QDomNode node; // XXX!!!
#endif

    QTextStream stream(stdout);
    stream << node;
    
    KoXmlElement n;
    forEachElement ( n, tableElement ) {
        if ( n.namespaceURI() != KoXmlNS::table )
            continue;

        if ( n.localName() == "table-rows" 
             || n.localName() == "table-header-rows" )
        {
            const bool isHeader = n.localName() == "table-header-rows";
            int           row = 0;

            KoXmlElement  _n;
            forEachElement ( _n, n ) {
                if ( _n.namespaceURI() != KoXmlNS::table )
                    continue;

                if ( _n.localName() == "table-row" ) {
                    int column = 0;
                    if ( !isHeader )
                        setRowCount( rowCount() + 1 );

                    KoXmlElement __n;
                    forEachElement ( __n, _n ) {
			if ( __n.namespaceURI() != KoXmlNS::table )
			    continue;

                        if ( __n.localName() == "table-cell" ) {
                            if ( column >= columnCount() )
                                setColumnCount( columnCount() + 1 );
#ifndef KOXML_USE_QDOM
                            const QString valueType = __n.attributeNS( KoXmlNS::office, "value-type" );
#else
                            const QString valueType; // XXX!!!
#endif

                            QString valueString;
#ifndef KOXML_USE_QDOM
                            const KoXmlElement valueElement = __n.namedItemNS( KoXmlNS::text, "p" ).toElement();
#else
                            const KoXmlElement valueElement; // XXX!!!
#endif
                            if ( valueElement.isNull() || !valueElement.isElement() ) {
                                qWarning() << "TableModel::loadOdf(): Cell contains no valid <text:p> element, cannnot load cell data.";
				// Even if it doesn't contain any value, it's still a cell.
				column++;
                                continue;
                            }

                            QVariant value;
                            valueString = valueElement.text();
                            if ( valueType == "float" )
                                value = valueString.toDouble();
                            else if ( valueType == "boolean" )
                                value = (bool)valueString.toInt();
                            else // if ( valueType == "string" )
                                value = valueString;

                            if ( isHeader )
                                setHeaderData( column, Qt::Horizontal, value );
                            else
                                setData( index( row, column ), value );
                            column++;
                        }
                    }
                }
                row++;
            }
        }
    }
}

bool TableModel::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    return true;
}

}
