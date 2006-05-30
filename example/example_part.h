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

#ifndef EXAMPLE_PART_H
#define EXAMPLE_PART_H

#include <KoDocument.h>

class ExamplePart : public KoDocument
{
    Q_OBJECT
public:
    ExamplePart( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool loadXML( QIODevice *, const QDomDocument & );
    virtual QDomDocument saveXML();

    virtual bool loadOasis( const QDomDocument & doc, KoOasisStyles& oasisStyles,
                            const QDomDocument & settings, KoStore* store );
    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
};

#endif
