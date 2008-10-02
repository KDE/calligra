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

static const struct PlaceholderData {
    const char * m_presentationClass;
    const char * m_shapeId;
    const char * m_xmlElement;
    const char * m_text;
} placeholderData[] = {
    { "title", "TextShapeID", "<draw:text-box/>", I18N_NOOP( "Double click to add a title" ) },
    { "outline", "TextShapeID", "<draw:text-box/>", I18N_NOOP( "Double click to add an outline" ) },
    { "subtitle", "TextShapeID", "<draw:text-box/>", I18N_NOOP( "Double click to add a text" ) },
    { "text", "TextShapeID", "<draw:text-box/>", I18N_NOOP( "Double click to add a text" ) },
    { "notes", "TextShapeID", "<draw:text-box/>", I18N_NOOP( "Double click to add notes" ) },
    { "graphic", "PictureShape", "<draw:image xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>", 
                                       I18N_NOOP( "Double click to add a picture" ) },
    { "chart", "ChartShape", "<draw:object xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>",
                                       I18N_NOOP( "Double click to add a chart" ) },
    { "object", "ChartShape", "<draw:object xlink:href=\"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>",
                                       I18N_NOOP( "Double click to add an object" ) }
};

static QMap<QString, const PlaceholderData *> s_placeholderMap;

void fillPlaceholderMap()
{
    const unsigned int numPlaceholderData = sizeof( placeholderData ) / sizeof( *placeholderData );
    for ( unsigned int i = 0; i < numPlaceholderData; ++i ) {
        s_placeholderMap.insert( placeholderData[i].m_presentationClass, &placeholderData[i] );
    }
}

KPrPlaceholderStrategy * KPrPlaceholderStrategy::create( const QString & presentationClass )
{
    if ( s_placeholderMap.isEmpty() ) {
        fillPlaceholderMap();
    }

    // TODO create a special strategy for pictures to show a dialog where the user can select a picture
    KPrPlaceholderStrategy * strategy = 0;
    if ( s_placeholderMap.contains( presentationClass ) ) {
        strategy = new KPrPlaceholderStrategy( presentationClass );
    }
    return strategy;
}

KPrPlaceholderStrategy::KPrPlaceholderStrategy( const QString & presentationClass )
: m_placeholderData( s_placeholderMap[presentationClass] )
{
}

KPrPlaceholderStrategy::~KPrPlaceholderStrategy()
{
}

KoShape * KPrPlaceholderStrategy::createShape( const QMap<QString, KoDataCenter *> & dataCenterMap )
{
    KoShape * shape = 0;
    KoShapeFactory * factory = KoShapeRegistry::instance()->value( m_placeholderData->m_shapeId );
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
    writer.addCompleteElement( m_placeholderData->m_xmlElement );
}

QString KPrPlaceholderStrategy::text() const
{
    return i18n( m_placeholderData->m_text );
}
