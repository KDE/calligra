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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"

#include <qpainter.h>

KPTPart::KPTPart( QWidget *parentWidget, const char *widgetName,
		  QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
    project = new KPTProject();
}

KPTPart::~KPTPart()
{
    delete project;
}

bool KPTPart::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

KoView* KPTPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KPTView( this, parent, name );
}

bool KPTPart::loadXML( QIODevice *, const QDomDocument & )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument KPTPart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void KPTPart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
//     int left = rect.left() / 20;
//     int right = rect.right() / 20 + 1;
//     int top = rect.top() / 20;
//     int bottom = rect.bottom() / 20 + 1;

//     for( int x = left; x < right; ++x )
//         painter.drawLine( x * 40, top * 20, 40 * 20, bottom * 20 );
//     for( int y = left; y < right; ++y )
//         painter.drawLine( left * 20, y * 20, right * 20, y * 20 );
}

#include "kptpart.moc"
