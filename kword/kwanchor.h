/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kwanchor_h
#define kwanchor_h

#include <qrichtext_p.h>
using namespace Qt3;
class KWTextDocument;

/**
 * An anchor is a special character, or 'custom item'.
 */
class KWAnchor : public QTextCustomItem
{
public:
    KWAnchor( KWTextDocument *textdoc );
    ~KWAnchor() {}

    QPoint origin();

    Placement placement() const { return PlaceInline; }
    // Currently no visual support
    void adjustToPainter( QPainter* ) {} // ####
    int widthHint() const { return 0; } // ####
    int minimumWidth() const { return 0; } // ####

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg );
private:
    // ###
};

#endif
