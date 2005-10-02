/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
 * Boston, MA 02110-1301, USA.
*/
#include "reportitem.h"

#include <klocale.h>
#include <kglobalsettings.h>

#include <qrect.h>
#include <qpainter.h>
#include <qregexp.h>

#include <koproperty/property.h>

#include "canvas.h"
#include "band.h"
#include "propertyserializer.h"

#include <kdebug.h>

namespace Kudesigner
{

QRect ReportItem::topLeftResizableRect()
{
    return QRect( ( int ) x(), ( int ) y(), HolderSize, HolderSize );
}

QRect ReportItem::bottomLeftResizableRect()
{
    return QRect( ( int ) x(), ( int ) ( y() + height() - HolderSize ), HolderSize, HolderSize );
}

QRect ReportItem::topRightResizableRect()
{
    return QRect( ( int ) ( x() + width() - HolderSize ), ( int ) y(), HolderSize, HolderSize );
}

QRect ReportItem::bottomRightResizableRect()
{
    return QRect( ( int ) ( x() + width() - HolderSize ), ( int ) ( y() + height() - HolderSize ), HolderSize, HolderSize );
}

QRect ReportItem::topMiddleResizableRect()
{
    return QRect( ( int ) ( x() + width() / 2 - HolderSize / 2. ), ( int ) y(), HolderSize, HolderSize );
}

QRect ReportItem::bottomMiddleResizableRect()
{
    return QRect( ( int ) ( x() + width() / 2 - HolderSize / 2. ), ( int ) ( y() + height() - HolderSize ), HolderSize, HolderSize );
}

QRect ReportItem::leftMiddleResizableRect()
{
    return QRect( ( int ) x(), ( int ) ( y() + height() / 2 - HolderSize / 2. ), HolderSize, HolderSize );
}

QRect ReportItem::rightMiddleResizableRect()
{
    return QRect( ( int ) ( x() + width() - HolderSize ), ( int ) ( y() + height() / 2 - HolderSize / 2. ), HolderSize, HolderSize );
}

void ReportItem::updateGeomProps()
{
    if ( !section() )
        return ;
    props[ "X" ].setValue( ( int ) ( x() - section() ->x() ) );
    props[ "Y" ].setValue( ( int ) ( y() - section() ->y() ) );
    props[ "Width" ].setValue( width() );
    props[ "Height" ].setValue( height() );
}

Band *ReportItem::section()
{
    return parentSection;
}

void ReportItem::setSection( Band *section )
{
    props[ "X" ].setValue( ( int ) ( x() - section->x() ) );
    props[ "Y" ].setValue( ( int ) ( y() - section->y() ) );
    parentSection = section;
}

void ReportItem::setSectionUndestructive( Band *section )
{
    parentSection = section;
}

QString ReportItem::getXml()
{
    QString result = "";

    int i = 1;
    for ( Set::Iterator it( props ); it.current(); ++it )
    {
        if ( !( i % 3 ) )
            result += "\n\t\t  ";
        result += " " + QString(it.currentKey()) + "=" + "\"" + escape( PropertySerializer::toString( it.current() ) ) + "\"";
        i++;
    }

    return result;
}

int ReportItem::isInHolder( const QPoint p )
{
    if ( topLeftResizableRect().contains( p ) )
        return ( ResizeTop | ResizeLeft );
    if ( bottomLeftResizableRect().contains( p ) )
        return ( ResizeBottom | ResizeLeft );
    if ( leftMiddleResizableRect().contains( p ) )
        return ( ResizeLeft );
    if ( bottomMiddleResizableRect().contains( p ) )
        return ( ResizeBottom );
    if ( topMiddleResizableRect().contains( p ) )
        return ( ResizeTop );
    if ( topRightResizableRect().contains( p ) )
        return ( ResizeTop | ResizeRight );
    if ( bottomRightResizableRect().contains( p ) )
        return ( ResizeBottom | ResizeRight );
    if ( rightMiddleResizableRect().contains( p ) )
        return ( ResizeRight );

    return ResizeNothing;
}

void ReportItem::drawHolders( QPainter &painter )
{
    painter.setPen( QColor( 0, 0, 0 ) );

    painter.setBrush( KGlobalSettings::highlightColor() );

    QCanvasItemList list = collisions( false );
    QCanvasItemList::iterator it = list.begin();
    for ( ; it != list.end(); ++it )
    {
        switch ( ( *it )->rtti() )
        {
        case Rtti_Label:
        case Rtti_Field:
        case Rtti_Special:
        case Rtti_Calculated:
        case Rtti_Line:
            {
            ReportItem *item = static_cast<ReportItem*>( *it );
            if ( section() != item->section() )
                continue;
            if ( intersects( item ) )
                painter.setBrush( Qt::red );
            break;
            }
        default:
            break;
        }
    }

    if ( props["Height"].value().toInt() > section()->props["Height"].value().toInt() )
        painter.setBrush( Qt::red );

    painter.drawRect( topLeftResizableRect() );
    painter.drawRect( topRightResizableRect() );
    painter.drawRect( bottomLeftResizableRect() );
    painter.drawRect( bottomRightResizableRect() );
    painter.drawRect( topMiddleResizableRect() );
    painter.drawRect( bottomMiddleResizableRect() );
    painter.drawRect( leftMiddleResizableRect() );
    painter.drawRect( rightMiddleResizableRect() );
}

bool ReportItem::intersects( ReportItem *item )
{
    QRect r1;
    QRect r2;

    if ( rtti() == Rtti_Line /*line*/ )
    {
        int x1 = props["X1"].value().toInt();
        int x2 = props["X2"].value().toInt();
        int y1 = props["Y1"].value().toInt();
        int y2 = props["Y2"].value().toInt();
        int width = props["Width"].value().toInt();
        //TODO I'm not sure of a good fix for this, but for now I'm assuming lines
        // in reports are either horizontal or vertical.
        if ( x1 == x2 )
            r1 = QRect( x1, y1, x2 + width, y2 );
        else if ( y1 == y2 )
            r1 = QRect( x1, y1, x2, y2 + width );
    }
    else
        r1 = QRect( props["X"].value().toInt(), props["Y"].value().toInt(),
                    props["Width"].value().toInt(), props["Height"].value().toInt() );

    if ( item->rtti() == Rtti_Line /*line*/ )
    {
        int x1 = item->props["X1"].value().toInt();
        int x2 = item->props["X2"].value().toInt();
        int y1 = item->props["Y1"].value().toInt();
        int y2 = item->props["Y2"].value().toInt();
        int width = item->props["Width"].value().toInt();
        //TODO I'm not sure of a good fix for this, but for now I'm assuming lines
        // in reports are either horizontal or vertical.
        if ( x1 == x2 )
            r2 = QRect( x1, y1, x2 + width, y2 );
        else if ( y1 == y2 )
            r2 = QRect( x1, y1, x2, y2 + width );
    }
    else
        r2 = QRect( item->props["X"].value().toInt(), item->props["Y"].value().toInt(),
                    item->props["Width"].value().toInt(), item->props["Height"].value().toInt() );

    bool intersects = r1.intersects( r2 );

    if ( intersects )
        kdDebug(30001) << rttiName( rtti() ) << " " << r1 << "\n"
                  << "...is intersected by..." << "\n"
                  << rttiName( item->rtti() ) << " " << r2
                  << endl;

    return intersects;
}

QString ReportItem::escape( QString string )
{
    string.replace( QRegExp( "&" ), "&amp;" );
    string.replace( QRegExp( "<" ), "&lt;" );
    string.replace( QRegExp( ">" ), "&gt;" );
    return string;
}

}
