// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef _kpresenter_utils_h__
#define _kpresenter_utils_h__

#include "global.h"
#include "koPointArray.h"
#include <qcursor.h>
class QPoint;
class QColor;
class QPainter;
class KoZoomHandler;
class KoSize;
class KoPoint;
class QDomElement;
class KPObject;

void drawFigure( LineEnd figure, QPainter* painter, const KoPoint &coord, const QColor & color,
                 int _w, float angle, KoZoomHandler*_zoomHandler );
void drawFigureWithOffset( LineEnd figure, QPainter* painter, const QPoint &coord, const QColor & color,
                           int _w, float angle, KoZoomHandler*_zoomHandler );
KoSize getBoundingSize( LineEnd figure, int _w, const KoZoomHandler*_zoomHandler );
KoSize getOffset( LineEnd figure, int _w, float angle );
QString lineEndBeginName( LineEnd type );
LineEnd lineEndBeginFromString( const QString & name );
KoPointArray getCloseObject( KoPointArray points, bool close, bool objClosed );
QString lineEndBeginSvg( LineEnd element );
QString saveOasisTimer( int second );
int loadOasisTimer( const QString & timer );

class KPresenterUtils
{
public:
    static QCursor penCursor(); // pen tool cursors
    static QCursor rotateCursor(); // rotate tool cursor
};

int readOasisSettingsInt( const QDomElement & element );
double readOasisSettingsDouble( const QDomElement & element );
bool readOasisSettingsBool( const QDomElement & element );
QString readOasisSettingsString( const QDomElement & element );

enum PropertyType {
    PtPen = 1,
    PtLineEnds = 2,
    PtBrush = 4,
    PtPie = 8,
    PtPicture = 16,
    PtPolygon = 32,
    PtRectangle = 64,
    PtOther = 128
};

int getPropertyFlags( const QPtrList<KPObject> &list );

#endif
