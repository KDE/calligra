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
    virtual ~KDChartTextPiece();

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
