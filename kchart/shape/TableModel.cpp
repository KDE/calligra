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

// KDE
#include <KDebug>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>

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

bool TableModel::setCellRegion(const QString& regionName)
{
    return false;
}

void TableModel::loadOdf( const KoXmlElement &tableElement, const KoOdfStylesReader &stylesReader )
{
    setRowCount( 0 );
    setColumnCount( 0 );
    
    KoXmlElement n = tableElement.firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling().toElement() )
    {
        kDebug(350001) << n.localName();
        if ( n.namespaceURI() != KoXmlNS::table )
            continue;
        if ( n.localName() == "table-rows" || n.localName() == "table-header-rows" )
        {
            const bool isHeader = n.localName() == "table-header-rows";
            int row = 0;
            KoXmlElement _n = n.firstChild().toElement();
            for ( ; !_n.isNull(); _n = _n.nextSibling().toElement() )
            {
                if ( _n.namespaceURI() != KoXmlNS::table )
                    continue;
                if ( _n.localName() == "table-row" )
                {
                    int column = 0;
                    if ( !isHeader )
                        setRowCount( rowCount() + 1 );
                    KoXmlElement __n = _n.firstChild().toElement();
                    for ( ; !__n.isNull(); __n = __n.nextSibling().toElement() )
                    {
                        if ( __n.namespaceURI() != KoXmlNS::table )
                            continue;
                        if ( __n.localName() == "table-cell" )
                        {
                            if ( isHeader )
                                setColumnCount( columnCount() + 1 );
                            const QString valueType = __n.attributeNS( KoXmlNS::office, "value-type" );
                            QString valueString;
                            const KoXmlElement valueElement = __n.namedItemNS( KoXmlNS::text, "p" ).toElement();
                            if ( valueElement.isNull() || !valueElement.isElement() )
                            {
                                qWarning() << "TableModel::loadOdf(): Cell contains no valid <text:p> element, cannnot load cell data.";
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
                            kDebug(350001) << isHeader;
                            kDebug(350001) << "Setting data in" << row << column << "to" << value;
                            column++;
                        }
                    }
                }
                row++;
            }
        }
    }
    
    kDebug(350001) << "BEFORE" << rowCount() << columnCount();
    //reset(); // the model does that for us, doesn't it?
    kDebug() << "AFTER" << rowCount() << columnCount();
    
    kDebug(350001) << "++++++++ " << rowCount() << columnCount();
}

bool TableModel::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    return true;
}

}
