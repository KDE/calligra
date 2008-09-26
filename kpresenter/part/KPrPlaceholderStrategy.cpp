/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrPlaceholderStrategy.h"

#include <QPainter>
#include <QPen>
#include <QTextOption>

#include <klocale.h>
#include <KoShapeFactory.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>

static QMap<QString, QPair<const char *, const char *> > s_placeholderMap;

static const struct {
    const char * m_presentationClass;
    const char * m_shapeId;
    const char * m_xmlElement;
    const char * m_text;
} placeholderData[] = {
    { "title", "TextShapeID", "<draw:textbox/>", I18N_NOOP( "Double click to add a title" ) },
    { "outline", "TextShapeID", "<draw:textbox/>", I18N_NOOP( "Double click to add an outline" ) },
    { "subtitle", "TextShapeID", "<draw:textbox/>", I18N_NOOP( "Double click to add a text" ) },
    { "text", "TextShapeID", "<draw:textbox/>", I18N_NOOP( "Double click to add a text" ) },
    { "notes", "TextShapeID", "<draw:textbox/>", I18N_NOOP( "Double click to add notes" ) },
    { "graphic", "PictureShape", "<draw:image xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>", 
                                       I18N_NOOP( "Double click to add a picture" ) },
    { "chart", "ChartShape", "<draw:object xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>",
                                       I18N_NOOP( "Double click to add a chart" ) },
    { "object", "ChartShape", "<draw:object xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>",
                                       I18N_NOOP( "Double click to add a chart" ) }
};


void fillPlaceholderMap()
{
    QPair<const char *, const char *> data;
    data.first = "TextShapeID";
    data.second = "<draw:textbox/>";
    s_placeholderMap.insert( "title", data );
    s_placeholderMap.insert( "outline", data );
    s_placeholderMap.insert( "subtitle", data );
    s_placeholderMap.insert( "text", data );
    s_placeholderMap.insert( "notes", data );
    data.first = "PictureShape";
    data.second = "<draw:image xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>";
    s_placeholderMap.insert( "graphic", data );
    data.first = "ChartShape";
    data.second = "<draw:object xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>";
    s_placeholderMap.insert( "chart", data );
    s_placeholderMap.insert( "object", data );
    /*
     * not yet supported
     * table
     * orgchart
     * page
     * handout
     * header
     * footer
     * date-time
     * page-number
    */
}

KPrPlaceholderStrategy * KPrPlaceholderStrategy::create( const QString & presentationClass )
{
    if ( s_placeholderMap.isEmpty() ) {
        fillPlaceholderMap();
    }

    // TODO create a special strategy for pictures to show a dialog where the user can select a picture
    KPrPlaceholderStrategy * strategy = 0;
    QMap<QString, QPair<const char *, const char *> >::const_iterator it( s_placeholderMap.find( presentationClass ) );
    if ( it != s_placeholderMap.end() ) {
        strategy = new KPrPlaceholderStrategy( it.value().first, it.value().second );
    }
    return strategy;
}

KPrPlaceholderStrategy::KPrPlaceholderStrategy( const char * shapeId, const char * xmlElement )
: m_shapeId( shapeId )
, m_xmlElement( xmlElement )
{
}

KPrPlaceholderStrategy::~KPrPlaceholderStrategy()
{
}

KoShape * KPrPlaceholderStrategy::createShape( const QMap<QString, KoDataCenter *> & dataCenterMap )
{
    KoShape * shape = 0;
    KoShapeFactory * factory = KoShapeRegistry::instance()->value( m_shapeId );
    Q_ASSERT( factory );
    if ( factory ) {
        shape = factory->createDefaultShapeAndInit( dataCenterMap );
    }
    return shape;
}

void KPrPlaceholderStrategy::paint( QPainter & painter, const QRectF & rect )
{
    QPen penText( Qt::black );
    painter.setPen( penText );
    //painter.setFont()
    QTextOption options( Qt::AlignCenter );
    options.setWrapMode( QTextOption::WordWrap );
    painter.drawText( rect, text(), options );
    QPen pen( Qt::gray );
    pen.setStyle( Qt::DashLine );
    painter.setPen( pen );
    painter.drawRect( rect );
}

void KPrPlaceholderStrategy::saveOdf( KoShapeSavingContext & context )
{
    KoXmlWriter & writer = context.xmlWriter();
    writer.addCompleteElement( m_xmlElement );
}

QString KPrPlaceholderStrategy::text() const
{
    return i18n( "Double click to insert ..." );
}
