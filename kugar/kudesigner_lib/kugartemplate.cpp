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
#include "kugartemplate.h"

#include <klocale.h>

#include <q3canvas.h>
#include <qpainter.h>
#include <qprinter.h>
#include <q3paintdevicemetrics.h>

#include <koproperty/property.h>

#include "propertyserializer.h"

#include "canvas.h"
#include "reportitem.h"

#include "reportheader.h"
#include "reportfooter.h"
#include "pageheader.h"
#include "pagefooter.h"
#include "detailheader.h"
#include "detailfooter.h"
#include "detail.h"

#include <kdebug.h>

namespace Kudesigner
{

KugarTemplate::KugarTemplate( int x, int y, int width, int height, Canvas *canvas )
        : Section( x, y, width, height, canvas )
{
    detailsCount = 0;

    setZ( 1 );
    m_canvas->setKugarTemplate( this );

    reportHeader = 0;
    reportFooter = 0;
    pageHeader = 0;
    pageFooter = 0;

    props.setGroupDescription( "DocumentSettings", i18n( "Document Settings" ) );

    QMap<QString, QString> m;
    m[ "A4" ] = "0";
    m[ "B5" ] = "1";
    m[ "Letter" ] = "2";
    m[ "Legal" ] = "3";
    m[ "Executive" ] = "4";
    m[ "A0" ] = "5";
    m[ "A1" ] = "6";
    m[ "A2" ] = "7";
    m[ "A3" ] = "8";
    m[ "A5" ] = "9";
    m[ "A6" ] = "10";
    m[ "A7" ] = "11";
    m[ "A8" ] = "12";
    m[ "A9" ] = "13";
    m[ "B0" ] = "14";
    m[ "B1" ] = "15";
    m[ "B10" ] = "16";
    m[ "B2" ] = "17";
    m[ "B3" ] = "18";
    m[ "B4" ] = "19";
    m[ "B6" ] = "20";
    m[ "B7" ] = "21";
    m[ "B8" ] = "22";
    m[ "B9" ] = "23";
    m[ "C5E" ] = "24";
    m[ "Comm10E" ] = "25";
    m[ "DLE" ] = "26";
    m[ "Folio" ] = "27";
    m[ "Ledger" ] = "28";
    m[ "Tabloid" ] = "29";
    m[ "NPageSize" ] = "30";
    props.addProperty( new Property( "PageSize", m.values(), m.keys(), "0", i18n( "Page Size" ), i18n( "Page Size" ) ), "DocumentSettings" );
    m.clear();

    m[ i18n( "Portrait" ) ] = "0";
    m[ i18n( "Landscape" ) ] = "1";
    props.addProperty( new Property( "PageOrientation", m.values(), m.keys(), "0", i18n( "Page Orientation" ), i18n( "Page Orientation" ) ), "DocumentSettings" );
    m.clear();

    props.addProperty( new Property( "TopMargin", 0, i18n( "Top Margin" ), i18n( "Top Margin" ), KoProperty::Integer ), "DocumentSettings" );
    props.addProperty( new Property( "BottomMargin", 0, i18n( "Bottom Margin" ), i18n( "Bottom Margin" ), KoProperty::Integer ), "DocumentSettings" );
    props.addProperty( new Property( "LeftMargin", 0, i18n( "Left Margin" ), i18n( "Left Margin" ), KoProperty::Integer ), "DocumentSettings" );
    props.addProperty( new Property( "RightMargin", 0, i18n( "Right Margin" ), i18n( "Right Margin" ), KoProperty::Integer ), "DocumentSettings" );
}

KugarTemplate::~KugarTemplate()
{
    if ( reportHeader )
        delete reportHeader;
    if ( pageHeader )
        delete pageHeader;

    std::map<int, DetailBand>::const_iterator it;
    for ( it = details.begin(); it != details.end(); ++it )
    {
        if ( it->second.first.first )
            delete it->second.first.first;
        if ( it->second.second )
            delete it->second.second;
        if ( it->second.first.second )
            delete it->second.first.second;
    }
    if ( pageFooter )
        delete pageFooter;
    if ( reportFooter )
        delete reportFooter;
}

void KugarTemplate::draw( QPainter &painter )
{
    updatePaperProps();
    painter.setPen( QPen( QColor( 160, 160, 160 ), 0, Qt::SolidLine ) );
    QPoint p1( ( int ) ( x() + props[ "LeftMargin" ].value().toInt() ),
               ( int ) ( y() + props[ "TopMargin" ].value().toInt() ) );
    QPoint p2( ( int ) ( x() + props[ "LeftMargin" ].value().toInt() ),
               ( int ) y() + height() - props[ "BottomMargin" ].value().toInt() );
    QPoint p3( ( int ) x() + width() - props[ "RightMargin" ].value().toInt(),
               ( int ) y() + height() - props[ "BottomMargin" ].value().toInt() );
    QPoint p4( ( int ) x() + width() - props[ "RightMargin" ].value().toInt(),
               ( int ) ( y() + props[ "TopMargin" ].value().toInt() ) );
    painter.drawLine(p1,p2);
    painter.drawLine(p2,p3);
	painter.drawLine(p3,p4);
	painter.drawLine(p4,p1);
    /*    painter.drawRect((int)(x()+props["LeftMargin"].first.toInt()),
            (int)(y()+props["TopMargin"].first.toInt()),
            width() - props["LeftMargin"].first.toInt() - props["RightMargin"].first.toInt(),
            height() - props["TopMargin"].first.toInt() - props["BottomMargin"].first.toInt());*/

    if ( Config::gridSize() > 1 )
    {
        int x = Config::gridSize();
        while ( x < width() )
        {
            int y = Config::gridSize();
            while ( y < height() )
            {
                painter.drawPoint( x, y );
                y += Config::gridSize();
            }
            x += Config::gridSize();
        }
    }

    Section::draw( painter );
}

void KugarTemplate::updatePaperProps()
{
    QPrinter * printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage( true );
    printer->setPageSize( ( QPrinter::PageSize ) props[ "PageSize" ].value().toInt() );
    printer->setOrientation( ( QPrinter::Orientation ) props[ "PageOrientation" ].value().toInt() );

    // Get the page metrics and set appropriate wigth and height
    Q3PaintDeviceMetrics pdm( printer );
    canvas() ->resize( pdm.width(), pdm.height() );
    setSize( pdm.width(), pdm.height() );

    //this is not needed anymore
    delete printer;
}

/*arrange sections on page automatically*/
void KugarTemplate::arrangeSections( bool destructive )
{
    int base = props[ "TopMargin" ].value().toInt();
    if ( reportHeader )
    {
        reportHeader->arrange( base, destructive );
        base += reportHeader->props[ "Height" ].value().toInt();
        reportHeader->show();
    }
    if ( pageHeader )
    {
        pageHeader->arrange( base, destructive );
        base += pageHeader->props[ "Height" ].value().toInt();
        pageHeader->show();
    }

    std::map<int, DetailBand>::const_iterator it;
    for ( it = details.begin(); it != details.end(); ++it )
    {
        //arranging detail header
        if ( it->second.first.first )
        {
            it->second.first.first->arrange( base, destructive );
            base += it->second.first.first->props[ "Height" ].value().toInt();
            it->second.first.first->show();
        }
        //arranging detail
        if ( it->second.second )
        {
            it->second.second->arrange( base, destructive );
            base += it->second.second->props[ "Height" ].value().toInt();
            it->second.second->show();
        }
    }
    std::map<int, DetailBand>::reverse_iterator itr;
    for ( itr = details.rbegin(); itr != details.rend(); ++itr )
    {
        //arranging detail footer
        if ( itr->second.first.second )
        {
            itr->second.first.second->arrange( base, destructive );
            base += itr->second.first.second->props[ "Height" ].value().toInt();
            itr->second.first.second->show();
        }
    }

    if ( pageFooter )
    {
        pageFooter->arrange( base, destructive );
        base += pageFooter->props[ "Height" ].value().toInt();
        pageFooter->show();
    }
    if ( reportFooter )
    {
        reportFooter->arrange( base, destructive );
        base += reportFooter->props[ "Height" ].value().toInt();
        reportFooter->show();
    }
}

QString KugarTemplate::getXml()
{
    QString result = "";
    result += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
    result += "<!DOCTYPE KugarTemplate SYSTEM \"kugartemplate.dtd\">\n\n";
    result += "<KugarTemplate";

    for ( Set::Iterator it( props ); it.current(); ++it )
    {
        QString attribute = it.currentKey();
        QString value = PropertySerializer::toString( it.current() );
        if ( !attribute.isEmpty() && !value.isEmpty() )
            result += " " + attribute + "=" + "\"" + value + "\"";
    }

    result += " PageWidth=\"" + QString::number( width() )
           + "\" PageHeight=\"" + QString::number( height() ) + "\"";

    result += ">\n";

    if ( reportHeader )
        result += reportHeader->getXml();
    if ( pageHeader )
        result += pageHeader->getXml();

    std::map<int, DetailBand>::const_iterator it;
    for ( it = details.begin(); it != details.end(); ++it )
    {
        //getting xml from detail header
        if ( it->second.first.first )
            result += it->second.first.first->getXml();
        //getting xml from detail
        if ( it->second.second )
            result += it->second.second->getXml();
        //getting xml from detail footer
        if ( it->second.first.second )
            result += it->second.first.second->getXml();
    }
    if ( pageFooter )
        result += pageFooter->getXml();
    if ( reportFooter )
        result += reportFooter->getXml();

    result += "</KugarTemplate>\n";

    return result;
}

bool KugarTemplate::removeReportItem( Q3CanvasItem *item )
{
    if ( item->rtti() > 2000 )
    {

        item->hide();
        ReportItem *ritem = dynamic_cast<ReportItem*>( item );
        if ( ritem != 0 )
        {
            ritem->section() ->items.remove( ritem );
            qWarning( "good" );
        }
        //delete item;
        //        section = 0;
        canvas() ->update();

        return true;
    }
    if ( item->rtti() > 1800 )
    {
        /*    if ((*it)->rtti() == Detail)
            {
                CanvasDetail *det = (CanvasDetail*)(*it);
                if ( det->props["Level"].first.toInt() <
                    ((MyCanvas*)(canvas()))->templ->detailsCount - 1)
                    return;
            }*/

        Band * section = dynamic_cast<Band *>( item );

        DetailHeader *header = 0;
        DetailFooter *footer = 0;
        removeSection( section, &header, &footer );
        section->hide();
        delete section;

        if ( header )
        {
            header->hide();
            delete header;
        }
        if ( footer )
        {
            footer->hide();
            delete footer;
        }
        arrangeSections();
        canvas() ->update();

        return true;
    }

    return false;
}

void KugarTemplate::removeSection( Band *section,
                                   DetailHeader **header, DetailFooter **footer )
{
    *header = 0;
    *footer = 0;
    if ( section == reportHeader )
        reportHeader = 0;
    if ( section == reportFooter )
        reportFooter = 0;
    if ( section == pageHeader )
        pageHeader = 0;
    if ( section == pageFooter )
        pageFooter = 0;
    for ( std::map<int, DetailBand>::iterator i = details.begin(); i != details.end(); ++i )
    {
        if ( i->second.second == section )
        {
            //delete not only detail but it's header and footer
            i->second.second = 0;
            *header = i->second.first.first;
            i->second.first.first = 0;
            *footer = i->second.first.second;
            i->second.first.second = 0;
            detailsCount--;
        }
        if ( i->second.first.first == section )
            i->second.first.first = 0;
        if ( i->second.first.second == section )
            i->second.first.second = 0;
    }
}

Band *KugarTemplate::band( Kudesigner::RttiValues type, int level )
{
    switch ( type )
    {
    case Rtti_ReportHeader:
        return reportHeader;
    case Rtti_PageHeader:
        return pageHeader;
    case Rtti_DetailHeader:
        return details[ level ].first.first;
    case Rtti_Detail:
        return details[ level ].second;
    case Rtti_DetailFooter:
        return details[ level ].first.second;
    case Rtti_PageFooter:
        return pageFooter;
    case Rtti_ReportFooter:
        return reportFooter;
    default:
        return 0;
    }
    return 0;
}

}
