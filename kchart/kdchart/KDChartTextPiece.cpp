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
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KDChartTextPiece.h"

#include <qstylesheet.h>
#include <qsimplerichtext.h>
#include <qfontmetrics.h>
#include <qpainter.h>

KDChartTextPiece::KDChartTextPiece( const QString& text, const QFont& font )
{
    if( QStyleSheet::mightBeRichText( text ) ) {
        _isRichText = true;
        _richText = new QSimpleRichText( text, font );
        _richText->adjustSize();
    } else {
        _isRichText = false;
        _richText = 0;
    }

    // These three are needed for both
    _metrics = new QFontMetrics( font );
    _text = text;
    _font = font;
}


KDChartTextPiece::KDChartTextPiece( const KDChartTextPiece& src )
{
    _isRichText = src._isRichText;
    if( src._richText ) {
        _richText = new QSimpleRichText( src._text, src._font );
        _richText->adjustSize();
    }

    // used for both
    _metrics = new QFontMetrics( *src._metrics );
    _text = src._text;
    _font = src._font;
}


KDChartTextPiece& KDChartTextPiece::operator=( const KDChartTextPiece& src )
{
    if( this != &src ) {
        _isRichText = src._isRichText;
        if( src._richText ) {
            _richText = new QSimpleRichText( src._text, src._font );
            _richText->adjustSize();
        }

        // used for both
        _metrics = new QFontMetrics( *src._metrics );
        _text = src._text;
        _font = src._font;
    }

    return *this;
}



KDChartTextPiece::~KDChartTextPiece()
{
    if( _richText )
        delete _richText;
    if( _metrics )
        delete _metrics;
}


int KDChartTextPiece::width() const
{
    if( _isRichText )
        return _richText->widthUsed();
    else
        return _metrics->width( _text );
}


int KDChartTextPiece::height() const
{
    if( _isRichText )
        return _richText->height();
    else
        return _metrics->height();
}


int KDChartTextPiece::fontLeading() const
{
    return _metrics->leading();
}


void KDChartTextPiece::draw( QPainter *p, int x, int y,
                             const QRegion& clipRegion,
                             const QColor& color,
                             const QBrush* paper ) const
{
    if( _isRichText ) {
        QColorGroup cg;
        cg.setColor( QColorGroup::Text, color );
        _richText->draw( p, x, y, clipRegion, cg, paper );
    } else {
        p->save();
        p->setFont( _font );
        if( paper )
            p->setBrush( *paper );
        p->setPen( color );
        p->setClipRegion( clipRegion );
        p->drawText( x, y + _metrics->ascent(), _text );
        p->restore();
    }
}


QString KDChartTextPiece::text() const
{
    return _text;
}


QFont KDChartTextPiece::font() const
{
    return _font;
}


bool KDChartTextPiece::isRichText() const
{
    return _isRichText;
}


