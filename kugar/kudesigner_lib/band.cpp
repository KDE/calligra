/* This file is part of the KDE project
Copyright (C) 2003-2004 Alexander Dymo <adymo@mksat.net>

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
#include "band.h"

#include <qcanvas.h>
#include <qrect.h>
#include <qpainter.h>

#include <kglobalsettings.h>

#include "propertyserializer.h"
#include "kugartemplate.h"
#include "canvas.h"
#include "reportitem.h"

namespace Kudesigner
{

Band::~Band()
{
    for ( QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it )
    {
        //  (*it)->hide();
        m_canvas->selected.remove( static_cast<Box*>( *it ) );
        ( *it ) ->setSelected( false );
        delete ( *it );
    }
    items.clear();
}

void Band::draw( QPainter &painter )
{
    setX( m_canvas->kugarTemplate() ->props[ "LeftMargin" ].value().toInt() );
    setSize( m_canvas->kugarTemplate() ->width()
             - m_canvas->kugarTemplate() ->props[ "RightMargin" ].value().toInt()
             - m_canvas->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
             props[ "Height" ].value().toInt() );
    Section::draw( painter );
}

//arrange band and all sublings (items)
void Band::arrange( int base, bool destructive )
{
    int diff = base - ( int ) y();
    setY( base );
    if ( !destructive )
        return ;
    for ( QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it )
    {
        ( *it ) ->moveBy( 0, diff );
        //  ( (CanvasReportItem *)(*it) )->updateGeomProps();
        m_canvas->update();
        ( *it ) ->hide();
        ( *it ) ->show();
    }
}

int Band::minHeight()
{
    int result = static_cast<int>( y() + 10 );
    for ( QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it )
    {
        result = QMAX( result, static_cast<int>( static_cast<QCanvasRectangle*>( *it ) ->y() +
                       static_cast<QCanvasRectangle*>( *it ) ->height() ) );
    }
    return result - static_cast<int>( y() );
}

QString Band::getXml()
{
    QString result = "";

    for ( Set::Iterator it( props ); it.current(); ++it )
    {
        result += " " + QString(it.currentKey()) + "=" + "\"" + PropertySerializer::toString( it.current() ) + "\"";
    }

    result += ">\n";
    for ( QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it )
    {
        result += static_cast<ReportItem*>( *it ) ->getXml();
    }
    return result;
}

int Band::isInHolder( const QPoint p )
{
    if ( bottomMiddleResizableRect().contains( p ) )
        return ( ResizeBottom );
    return ResizeNothing;
}

void Band::drawHolders( QPainter &painter )
{
    painter.setPen( QColor( 0, 0, 0 ) );
    painter.setBrush( KGlobalSettings::highlightColor() );
    painter.drawRect( bottomMiddleResizableRect() );
}

QRect Band::bottomMiddleResizableRect()
{
    return QRect( ( int ) ( x() + width() / 2 - HolderSize / 2. ), ( int ) ( y() + height() - HolderSize ), HolderSize, HolderSize );
}

void Band::updateGeomProps()
{
    props[ "Height" ].setValue( height() );
    m_canvas->kugarTemplate() ->arrangeSections();
}

}
