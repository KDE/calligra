/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
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
#include <qapplication.h>
#include <qrect.h>

KDChartTextPiece::KDChartTextPiece()
    :QObject(0)
{
    _isRichText = false;
    _richText = 0;

    _font = QApplication::font();
    // These three are needed for both
    _metrics = new QFontMetrics( _font );
    _dirtyMetrics = true;
    _text = QString("");
}


KDChartTextPiece::KDChartTextPiece( const QString& text, const QFont& font )
    :QObject(0)
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
    _dirtyMetrics = true;
    _text = text;
    _font = font;
}


KDChartTextPiece::KDChartTextPiece( QPainter *p, const QString& text, const QFont& font )
    :QObject(0)
{
    
    if( QStyleSheet::mightBeRichText( text ) ) {
        _isRichText = true;
        _richText = new QSimpleRichText( text, font );
        //qDebug( "richtext width %s", QString::number(_richText->width()).latin1());
	//qDebug( "richtext height %s", QString::number(_richText->height()).latin1());
        _richText->adjustSize();
        //qDebug( "richtext width %s", QString::number(_richText->width()).latin1());
	//qDebug( "richtext height %s", QString::number(_richText->height()).latin1());
        
    } else {
        _isRichText = false;
        _richText = 0;
    }

    // These three are needed for both
    _dirtyMetrics = (p == 0);
    if( _dirtyMetrics ) {
        _metrics = new QFontMetrics( font );
        //qDebug("dirty metrics text: %s", text.latin1());
    }
    else{
        p->save();
        p->setFont( font );
        _metrics = new QFontMetrics( p->fontMetrics() );
        //qDebug ( "drawing metrics text: %s", text.latin1() );
        //p->drawRect( _metrics->boundingRect( text) );
	//p->drawText( _metrics->boundingRect(text).bottomRight(), text);
        p->restore();
    }
    _text = text;
    _font = font;
}


void KDChartTextPiece::deepCopy( const KDChartTextPiece* source )
{
    if( !source || this == source )
        return;
    if( _richText )
        delete _richText;
    _isRichText = source->_isRichText;
    if( source->_richText ) {
        _richText = new QSimpleRichText( source->_text, source->_font );
        _richText->adjustSize();
    }
    else
        _richText = 0;

    // used for both
    if( _metrics )
        delete _metrics;
    _metrics = new QFontMetrics( *source->_metrics );
    _dirtyMetrics = source->_dirtyMetrics;
    _text = source->_text;
    _font = source->_font;
}

const KDChartTextPiece* KDChartTextPiece::clone() const
{
    KDChartTextPiece* newPiece = new KDChartTextPiece();
    newPiece->deepCopy( this );
    return newPiece;
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
  
  if( _isRichText ) {
    //qDebug ("_richText height %s", QString::number(_richText->height()).latin1()); 
        return _richText->height();
  }
  else {
  
    //qDebug ("_metrics height %s", QString::number(_metrics->height()).latin1()); 
        return _metrics->height();
	 }
}


int KDChartTextPiece::fontLeading() const
{
    return _metrics->leading();
}

QRect KDChartTextPiece::rect( QPainter *p, const QRect& clipRect) const
{
    QRect rect( clipRect );
    QFont font( _font );
     
    if( _isRichText ) {  
     
    // Pending Michel make sure the fonts are not too large
      if ( _richText->height() > clipRect.height() || _richText->width() > clipRect.width() ) 
	font.setPixelSize( QMIN( (int)clipRect.width(),(int)clipRect.height() ) );

      _richText->setDefaultFont( font );
      _richText->setWidth( p, clipRect.width() ); 
      rect.setWidth( _richText->width() );
      rect.setHeight( _richText->height() );
    } else 
      rect = clipRect;
        
    return rect;
}

void KDChartTextPiece::draw( QPainter *p, int x, int y,
        const QRect& clipRect,
        const QColor& color,
        const QBrush* paper ) const
{
  
  if( _isRichText ) {
    QColorGroup cg;
    //calculate the text area before drawing
    QRect txtArea = rect( p,clipRect); 
    QRect rect;
    cg.setColor( QColorGroup::Text, color );
    // adjust the vertical position of the text within the area - we send a null rectangle to avoid clipping
    //PENDING: Michel - TODO - Let the user set or unset the adjustment factor by himself
   _richText->draw( p, txtArea.x(), txtArea.y() + (int)txtArea.height()/10 , rect, cg, paper );
  } else {
    p->save();
    p->setFont( _font );
    if( paper )
      p->setBrush( *paper );
    p->setPen( color );
    //dont clip to avoid truncated text 
    //p->setClipRect( txtArea );
    if( _dirtyMetrics ){
      if( _metrics )
	delete _metrics;
      KDChartTextPiece* meNotConst = const_cast<KDChartTextPiece*>(this);
      //KDChartTextPiece* meNotConst(const_cast<KDChartTextPiece*>(this));
      meNotConst->_metrics = new QFontMetrics( p->fontMetrics() );
      meNotConst->_dirtyMetrics = false;
    }
   
    p->drawText( x, y + _metrics->ascent(), _text );
    p->restore();
    }
}


void KDChartTextPiece::draw( QPainter *p, int x, int y,
        const QRegion& clipRegion,
        const QColor& color,
        const QBrush* paper ) const
{
    if( _isRichText ) {
        QColorGroup cg;
        cg.setColor( QColorGroup::Text, color );
        _richText->setDefaultFont( _font );
        _richText->setWidth( p, clipRegion.boundingRect().width() );
        _richText->draw( p, x, y, clipRegion, cg, paper );
    } else {
        p->save();
        p->setFont( _font );
        if( paper )
            p->setBrush( *paper );
        p->setPen( color );
        p->setClipRegion( clipRegion );

        if( _dirtyMetrics ){
            if( _metrics )
                delete _metrics;

            // this line does not compile with MSVC++:
            // KDChartTextPiece* meNotConst( const_cast<KDChartTextPiece*>(this) );
            KDChartTextPiece* meNotConst = const_cast<KDChartTextPiece*>(this);

            meNotConst->_metrics = new QFontMetrics( p->fontMetrics() );
            meNotConst->_dirtyMetrics = false;
        }

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



#include "KDChartTextPiece.moc"
