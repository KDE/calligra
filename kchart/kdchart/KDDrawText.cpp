/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#include <qpainter.h>
#include <math.h>

#include <KDDrawText.h>

void KDDrawText::drawRotatedText( QPainter* painter,
                                  float  degrees,
                                  QPoint anchor,
                                  const QString& text,
                                  const QFont* font,
                                  int align,
                                  bool showAnchor )
{
    drawRotatedTxt( painter,
                    degrees,
                    anchor,
                    text,
                    font,
                    align,
                    showAnchor );
}


QRegion KDDrawText::measureRotatedText( QPainter* painter,
                                        float  degrees,
                                        QPoint anchor,
                                        const QString& text,
                                        const QFont* font,
                                        int align )
{
    QRegion region;
    drawRotatedTxt( painter,
                    degrees,
                    anchor,
                    text,
                    font,
                    align,
                    false,
                    INT_MAX,
                    INT_MAX,
                    true,
                    &region );
    return region;
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
                                 bool calculateOnly,
                                 QRegion* region )
{
    bool fontChanged = ( 0 != font );
    QFont oldFont;
    if( fontChanged ) {
        oldFont = painter->font();
        painter->setFont( *font );
    }
    else
        font = &painter->font();
    QFontMetrics fm( *font );

    painter->rotate( degrees );
    QPoint pos( anchor );
    pos = painter->xFormDev( pos );

    if( INT_MAX == txtWidth ) {
        QFontMetrics fm( painter->font() );
        txtWidth  = fm.width(  text );
        txtHeight = fm.height();
    }
    else if( INT_MAX == txtHeight ) {
        QFontMetrics fm( painter->font() );
        txtHeight = fm.height();
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
    int x = pos.x();
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
    int y = pos.y();
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
    if( region ) {
        QRect rect( painter->boundingRect( x, y,
                                           txtWidth, txtHeight,
                                           Qt::AlignLeft + Qt::AlignTop,
                                           text ) );
        QPoint topLeft(     painter->xForm( rect.topLeft()     ) );
        QPoint topRight(    painter->xForm( rect.topRight()    ) );
        QPoint bottomRight( painter->xForm( rect.bottomRight() ) );
        QPoint bottomLeft(  painter->xForm( rect.bottomLeft()  ) );
        QPointArray points;
        points.setPoints( 4, topLeft.x(),     topLeft.y(),
                             topRight.x(),    topRight.y(),
                             bottomRight.x(), bottomRight.y(),
                             bottomLeft.x(),  bottomLeft.y() );
        *region = QRegion( points );
    }
    if( !calculateOnly )
        painter->drawText( x, y,
                           txtWidth, txtHeight,
                           Qt::AlignLeft + Qt::AlignTop,
                           text );
    painter->rotate( -degrees );
    if( fontChanged )
        painter->setFont( oldFont );
}


