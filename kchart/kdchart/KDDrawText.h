/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDDRAWTEXT_H__
#define __KDDRAWTEXT_H__

#include <qpainter.h>

class KDDrawText
{
public:
    static void drawRotatedText( QPainter* painter,
                                 float  degrees,
                                 QPoint anchor,
                                 const QString& text,
                                 const QFont* font = 0,
                                 int align         = Qt::AlignLeft | Qt::AlignTop,
                                 // usefull for debugging of your layout:
                                 bool showAnchor   = false );

    static QRegion measureRotatedText( QPainter* painter,
                                       float  degrees,
                                       QPoint anchor,
                                       const QString& text,
                                       const QFont* font,
                                       int align );

private:
    static void drawRotatedTxt( QPainter* painter,
                                float  degrees,
                                QPoint anchor,
                                const QString& text,
                                const QFont* font  = 0,
                                int align          = Qt::AlignLeft | Qt::AlignTop,
                                // usefull for debugging of your layout:
                                bool showAnchor    = false,
                                // speed-up parameters
                                // used internally by drawCircleText()
                                // to avoid duplicate calculation
                                int txtWidth       = INT_MAX,
                                int txtHeight      = INT_MAX,
                                bool calculateOnly = false,
                                QRegion* region    = 0 );

};

#endif
