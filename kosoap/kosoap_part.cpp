/* This file is part of the KDE project
   Copyright (C) 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   This is based off of the KOffice Example found in the KOffice
   CVS.  Torben Weis <weis@kde.org> is the original author.

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kosoap_part.h"
#include "kosoap_view.h"

#include <qpainter.h>

KOSoapPart::KOSoapPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
}

bool KOSoapPart::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

KoView* KOSoapPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KOSoapView( this, parent, name );
}

bool KOSoapPart::loadXML( QIODevice *, const QDomDocument & )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument KOSoapPart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void KOSoapPart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
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

#include "kosoap_part.moc"
