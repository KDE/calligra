/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTTEXTPIECE_H__
#define __KDCHARTTEXTPIECE_H__

#include <qstring.h>
#include <qfont.h>
#include <qregion.h>
#include <qpalette.h>

class QBrush;
class QPainter;
class QSimpleRichText;
class QFontMetrics;

class KDChartTextPiece
{
public:
    KDChartTextPiece( const QString& text, const QFont& font );
    KDChartTextPiece( const KDChartTextPiece& src );
    KDChartTextPiece& operator=( const KDChartTextPiece& src );
    ~KDChartTextPiece();

    virtual int width() const;
    virtual int height() const;
    virtual int fontLeading() const;

    virtual void draw( QPainter *p, int x, int y, const QRegion& clipRegion,
                       const QColor& color, const QBrush* paper = 0 ) const;

    QString text() const;
    QFont font() const;
    bool isRichText() const;

protected:
    bool _isRichText;
    QSimpleRichText* _richText; // used when _isRichText == true
    QString _text; // used when _isRichText == false
    QFont _font; // used when _isRichText == false
    QFontMetrics* _metrics;
};

#endif
