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
#include <qpainter.h>
#include <math.h>

#include <KDDrawText.h>

void KDDrawText::drawRotatedText( QPainter* painter,
                                  float  degrees,
                                  QPoint anchor,
                                  const QString& text,
                                  const QFont* font,
                                  int align,
                                  bool showAnchor,
                                  const QFontMetrics* fontMet,
                                  bool noFirstrotate,
                                  bool noBackrotate,
                                  KDDrawTextRegionAndTrueRect* infos )
{
    drawRotatedTxt( painter,
                    degrees,
                    anchor,
                    text,
                    font,
                    align,
                    showAnchor,
                    INT_MAX,
                    INT_MAX,
                    fontMet,
                    false,
                    0 != infos,
                    noFirstrotate,
                    noBackrotate,
                    infos );
}


KDDrawTextRegionAndTrueRect KDDrawText::measureRotatedText(
    QPainter* painter,
    float  degrees,
    QPoint anchor,
    const QString& text,
    const QFont* font,
    int align,
    const QFontMetrics* fontMet,
    bool noFirstrotate,
    bool noBackrotate,
    int addPercentOfHeightToRegion )
{
    KDDrawTextRegionAndTrueRect infos;
    drawRotatedTxt( painter,
                    degrees,
                    anchor,
                    text,
                    font,
                    align,
                    false,
                    INT_MAX,
                    INT_MAX,
                    fontMet,
                    true,
                    false,
                    noFirstrotate,
                    noBackrotate,
                    &infos,
                    addPercentOfHeightToRegion );
    return infos;
}


void KDDrawText::drawRotatedTxt( QPainter* painter,
                                 float  degrees,
                                 QPoint anchor,
                                 const QString& text,
                                 const QFont* font,
                                 int align,
                                 bool showAnchor,
                                 int txtWidth,
                                 int txtHeight,
                                 const QFontMetrics* fontMet,
                                 bool calculateOnly,
                                 bool doNotCalculate,
                                 bool noFirstrotate,
                                 bool noBackrotate,
                                 KDDrawTextRegionAndTrueRect* infos,
                                 int addPercentOfHeightToRegion )
{
    bool useInfos = doNotCalculate && infos;
    bool fontChanged = ( 0 != font );
    QFont oldFont;
    if( fontChanged ) {
        oldFont = painter->font();
        painter->setFont( *font );
    }
    else
        font = &painter->font();

    if( !noFirstrotate )
      painter->rotate( degrees );

    QPoint pos = useInfos ? infos->pos : painter->xFormDev( anchor );
    if( useInfos )
    {
        txtWidth  = infos->width;
        txtHeight = infos->height;
    }
    else
    {
        if( INT_MAX == txtWidth ) {
            if( fontMet ) {
              txtWidth  = fontMet->width(  text );
              txtHeight = fontMet->height();
            } else {
              QFontMetrics fm( painter->font() );
              txtWidth  = fm.width(  text );
              txtHeight = fm.height();
            }
        }
        else if( INT_MAX == txtHeight ) {
            if( fontMet )
              txtHeight = fontMet->height();
            else {
              QFontMetrics fm( painter->font() );
              txtHeight = fm.height();
            }
        }
        if( infos ) {
            infos->pos    = pos;
            infos->width  = txtWidth;
            infos->height = txtHeight;
        }
    }
    if( showAnchor ) {
        int d = txtHeight/4;
        QPen savePen = painter->pen();
        painter->setPen( QColor( Qt::darkRed ) );
        painter->drawLine( pos.x(),   pos.y()-d,
                           pos.x(),   pos.y()+d );
        painter->drawLine( pos.x()-d, pos.y(),
                           pos.x()+d, pos.y() );
        painter->setPen( savePen );
    }
    int x = useInfos ? infos->x : pos.x();
    int y = useInfos ? infos->y : pos.y();
    if( !useInfos ) {
        switch( align & ( Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter ) ) {
        case Qt::AlignLeft:
                    break;
        case Qt::AlignRight:
                    x -= txtWidth;
                    break;
        case Qt::AlignHCenter:
                    x -= txtWidth - txtWidth/2;
                    break;
        }
        switch( align & ( Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter ) ) {
        case Qt::AlignTop:
                    break;
        case Qt::AlignBottom:
                    y -= txtHeight;
                    break;
        case Qt::AlignVCenter:
                    y -= txtHeight/2;
                    break;
        }
    }
    if( infos && !useInfos ) {
        infos->x = x;
        infos->y = y;
        QRect rect( painter->boundingRect( x, y,
                                           txtWidth, txtHeight,
                                           Qt::AlignLeft + Qt::AlignTop,
                                           text ) );
        QPoint topLeft(     painter->xForm( rect.topLeft()     ) );
        QPoint topRight(    painter->xForm( rect.topRight()    ) );
        QPoint bottomRight( painter->xForm( rect.bottomRight() ) );
        QPoint bottomLeft(  painter->xForm( rect.bottomLeft()  ) );
        int additor = addPercentOfHeightToRegion * txtHeight / 100;
        QPointArray points;
        points.setPoints( 4, topLeft.x()-additor,     topLeft.y()-additor,
                             topRight.x()+additor,    topRight.y()-additor,
                             bottomRight.x()+additor, bottomRight.y()+additor,
                             bottomLeft.x()-additor,  bottomLeft.y()+additor );
        infos->region = QRegion( points );
    }
    if( showAnchor ) {
        int d = txtHeight/4;
        QPen savePen = painter->pen();
        painter->setPen( QColor( Qt::blue ) );
        painter->drawLine( x,   y-d,
                           x,   y+d );
        painter->drawLine( x-d, y,
                           x+d, y );
        painter->setPen( savePen );
    }
    if( !calculateOnly )
        painter->drawText( x, y,
                           txtWidth, txtHeight,
                           Qt::AlignLeft + Qt::AlignTop,
                           text );

    if( !noBackrotate )
      painter->rotate( -degrees );

    if( fontChanged )
        painter->setFont( oldFont );
}


