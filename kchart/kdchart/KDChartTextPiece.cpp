/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KDChart library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KDChart licenses may use this file in
** accordance with the KDChart Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
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
        if( _richText )
            delete _richText;
        _isRichText = src._isRichText;
        if( src._richText ) {
            _richText = new QSimpleRichText( src._text, src._font );
            _richText->adjustSize();
        }
        else
            _richText = 0;

        // used for both
        if( _metrics )
            delete _metrics;
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


