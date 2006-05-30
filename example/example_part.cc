/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "example_part.h"
#include "example_factory.h"
#include "example_view.h"

#include <qpainter.h>

ExamplePart::ExamplePart( QWidget *parentWidget, QObject* parent, bool singleViewMode )
    : KoDocument( parentWidget, parent, singleViewMode )
{
    setInstance( ExampleFactory::global(), false );
}

KoView* ExamplePart::createViewInstance( QWidget* parent, const char* name )
{
    return new ExampleView( this, parent, name );
}

bool ExamplePart::loadXML( QIODevice *, const QDomDocument & )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument ExamplePart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}

bool ExamplePart::loadOasis( const QDomDocument & doc, KoOasisStyles& oasisStyles,
		const QDomDocument & settings, KoStore* store )
{
    // TODO load the document from the QDomDocument
    return true;
}

bool ExamplePart::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
    // TODO save the document to the KoStore;
    return true;
}

void ExamplePart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
    int left = rect.left() / 20;
    int right = rect.right() / 20 + 1;
    int top = rect.top() / 20;
    int bottom = rect.bottom() / 20 + 1;

    for( int x = left; x < right; ++x )
        painter.drawLine( x * 20, top * 20, x * 20, bottom * 20 );
    for( int y = left; y < right; ++y )
        painter.drawLine( left * 20, y * 20, right * 20, y * 20 );
}

#include "example_part.moc"
