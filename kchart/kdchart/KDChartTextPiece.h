/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
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
