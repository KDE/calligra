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

//#include "kotextformat.h"
#include "qrichtext_p.h" // for KoTextFormat
#include "kozoomhandler.h"
#include <kdebug.h>

void KoTextFormat::KoTextFormatPrivate::clearCache()
{
    delete m_screenFontMetrics; m_screenFontMetrics = 0L;
    delete m_screenFont; m_screenFont = 0L;
    delete m_refFontMetrics; m_refFontMetrics = 0L;
    delete m_refFont; m_refFont = 0L;
    m_refAscent = -1;
    m_refDescent = -1;
    m_refHeight = -1;
    memset( m_screenWidths, 0, 256 * sizeof( ushort ) );
}

void KoTextFormat::setPointSizeFloat( float size )
{
    if ( fn.pointSizeFloat() == size )
        return;
    fn.setPointSizeFloat( size );
    update();
}

void KoTextFormat::setStrikeOutLineType (StrikeOutLineType _type)
{
    if ( m_strikeOutLine == _type )
        return;
    m_strikeOutLine = _type;
    update();
}

void KoTextFormat::setUnderlineLineType (UnderlineLineType _type)
{
    if ( m_underlineLine == _type )
        return;
    m_underlineLine = _type;
    update();
}

void KoTextFormat::setUnderlineLineStyle (UnderlineLineStyle _type)
{
    if ( m_underlineLineStyle == _type )
        return;
    m_underlineLineStyle = _type;
    update();
}

void KoTextFormat::setStrikeOutLineStyle( StrikeOutLineStyle _type )
{
    if ( m_strikeOutLineStyle == _type )
        return;
    m_strikeOutLineStyle = _type;
    update();
}

void KoTextFormat::setTextBackgroundColor(const QColor &_col)
{
    if(m_textBackColor==_col)
        return;
    m_textBackColor=_col;
    update();
}
void KoTextFormat::setTextUnderlineColor(const QColor &_col)
{
    if ( m_textUnderlineColor == _col )
        return;
    m_textUnderlineColor=_col;
    update();
}

void KoTextFormat::setShadowText(bool _b)
{
    if ( d->m_bShadowText == _b )
        return;
    d->m_bShadowText=_b;
    update();
}

void KoTextFormat::setRelativeTextSize( double _size )
{
    if ( d->m_relativeTextSize == _size)
        return;
    d->m_relativeTextSize = _size;
    update();
}

void KoTextFormat::setOffsetFromBaseLine( int _offset )
{
    if ( d->m_offsetFromBaseLine == _offset)
        return;
    d->m_offsetFromBaseLine = _offset;
    update();
}

void KoTextFormat::setWordByWord( bool _b )
{
    if ( d->m_bWordByWord == _b)
        return;
    d->m_bWordByWord = _b;
    update();
}


void KoTextFormat::setAttributeFont(KoTextFormat::AttributeStyle _att )
{
    if ( m_attributeFont == _att)
        return;
    m_attributeFont = _att;
    update();

}

int KoTextFormat::compare( const KoTextFormat & format ) const
{
    int flags = 0;
    if ( fn.weight() != format.fn.weight() )
        flags |= KoTextFormat::Bold;
    if ( fn.italic() != format.fn.italic() )
        flags |= KoTextFormat::Italic;
    if ( textUnderlineColor()!=format.textUnderlineColor() ||
         underlineLineType()!= format.underlineLineType() ||
         underlineLineStyle() != format.underlineLineStyle())
        flags |= KoTextFormat::ExtendUnderLine;
    if ( fn.family() != format.fn.family() )
        flags |= KoTextFormat::Family;
    if ( fn.pointSize() != format.fn.pointSize() )
        flags |= KoTextFormat::Size;
    if ( color() != format.color() )
        flags |= KoTextFormat::Color;
    if ( vAlign() != format.vAlign() ||
        relativeTextSize() != format.relativeTextSize())
        flags |= KoTextFormat::VAlign;
    if ( strikeOutLineType() != format.strikeOutLineType()
        || underlineLineStyle() != format.underlineLineStyle())
        flags |= KoTextFormat::StrikeOut;
    if ( textBackgroundColor() != format.textBackgroundColor() )
        flags |= KoTextFormat::TextBackgroundColor;
    if ( spellCheckingLanguage() != format.spellCheckingLanguage() )
        flags |= KoTextFormat::SpellCheckingLanguage;
    if ( shadowText() != format.shadowText() )
        flags |= KoTextFormat::ShadowText;
    if ( offsetFromBaseLine() != format.offsetFromBaseLine() )
        flags |= KoTextFormat::OffsetFromBaseLine;
    if ( wordByWord() != format.wordByWord() )
        flags |= KoTextFormat::WordByWord;
    if ( attributeFont() != format.attributeFont() )
        flags |= KoTextFormat::Attribute;
    return flags;
}

QColor KoTextFormat::defaultTextColor( QPainter * painter )
{
    if ( painter->device()->devType() == QInternal::Printer )
        return Qt::black;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Text );
}

float KoTextFormat::screenPointSize( const KoZoomHandler* zh ) const
{
    int pointSizeLU = font().pointSize();
    if ( vAlign() != KoTextFormat::AlignNormal )
        pointSizeLU = (int)( pointSizeLU *relativeTextSize() );

    return zh->layoutUnitToFontSize( pointSizeLU, false /* forPrint */ );
}

float KoTextFormat::refPointSize() const
{
    int pointSizeLU = font().pointSize();
    if ( vAlign() != KoTextFormat::AlignNormal )
        pointSizeLU = (int)( pointSizeLU * relativeTextSize());
    return KoTextZoomHandler::layoutUnitPtToPt( pointSizeLU );
}

QFont KoTextFormat::refFont() const
{
    float pointSize = refPointSize();
    if ( !d->m_refFont || pointSize != d->m_refFont->pointSizeFloat() )
    {
        delete d->m_refFont;
        d->m_refFont = new QFont( font() );
        d->m_refFont->setPointSizeFloat( pointSize );
        //kdDebug(32500) << "KoTextFormat::screenFont created new font with size " << pointSize << endl;
    }
    return *d->m_refFont;
}

QFont KoTextFormat::screenFont( const KoZoomHandler* zh ) const
{
    float pointSize = screenPointSize( zh );
    //kdDebug(32500) << "KoTextFormat::screenFont applyZoom=" << applyZoom << " pointSize=" << pointSize << endl;
    // Compare if this is the size for which we cached the font metrics.
    // We have to do this very dynamically, because 2 views could be painting the same
    // stuff, with different zoom levels. So no absolute caching possible.
    /*if ( d->m_screenFont )
      kdDebug(32500) << " d->m_screenFont->pointSizeFloat()=" << d->m_screenFont->pointSizeFloat() << endl;*/
    if ( !d->m_screenFont || pointSize != d->m_screenFont->pointSizeFloat() )
    {
        delete d->m_screenFont;
        d->m_screenFont = new QFont( font() );
        d->m_screenFont->setPointSizeFloat( pointSize );
        //kdDebug(32500) << "KoTextFormat::screenFont created new font with size " << pointSize << endl;
    }
    return *d->m_screenFont;
}

const QFontMetrics& KoTextFormat::screenFontMetrics( const KoZoomHandler* zh ) const
{
    float pointSize = screenPointSize( zh );
    if ( !d->m_screenFont )
        (void)screenFont( zh ); // we need it below, and this way it'll be ready for painting

    // Compare if this is the size for which we cached the font metrics.
    // We have to do this very dynamically, because 2 views could be painting the same
    // stuff, with different zoom levels. So no absolute caching possible.
    if ( !d->m_screenFontMetrics || pointSize != d->m_screenFont->pointSizeFloat() )
    {
        //kdDebug(32500) << this << " KoTextFormat::screenFontMetrics pointSize=" << pointSize << " d->m_screenFont->pointSizeFloat()=" << d->m_screenFont->pointSizeFloat() << endl;
        QFont f( font() );
        f.setPointSizeFloat( pointSize );
        delete d->m_screenFontMetrics;
        d->m_screenFontMetrics = new QFontMetrics( f );
        //kdDebug(32500) << "KoTextFormat::screenFontMetrics created new metrics with size " << pointSize << "   height:" << d->m_screenFontMetrics->height() << endl;
    }
    return *d->m_screenFontMetrics;
}

const QFontMetrics& KoTextFormat::refFontMetrics() const
{
    float pointSize = refPointSize();
    if ( !d->m_refFont )
        (void)refFont(); // we need it below, and this way it'll be ready for painting (errrr....)

    if ( !d->m_refFontMetrics || pointSize != d->m_refFont->pointSizeFloat() )
    {
        //kdDebug(32500) << this << " KoTextFormat::refFontMetrics pointSize=" << pointSize << " d->m_refFont->pointSizeFloat()=" << d->m_refFont->pointSizeFloat() << endl;
        QFont f( font() );
        f.setPointSizeFloat( pointSize );
        delete d->m_refFontMetrics;
        d->m_refFontMetrics = new QFontMetrics( f );
        //kdDebug(32500) << "KoTextFormat::refFontMetrics created new metrics with size " << pointSize << "   height:" << d->m_refFontMetrics->height() << endl;
    }
    return *d->m_refFontMetrics;
}

int KoTextFormat::charWidth( const KoZoomHandler* zh, bool applyZoom, const KoTextStringChar* c,
                             const KoTextParag* parag, int i ) const
{
    ushort unicode = c->c.unicode();
    if ( unicode == 0xad ) // soft hyphen
	return 0;
    Q_ASSERT( !c->isCustom() ); // actually it's a bit stupid to call this for custom items
    if( c->isCustom() ) {
	 if( c->customItem()->placement() == KoTextCustomItem::PlaceInline ) {
             // customitem width is in LU pixels. Convert to 100%-zoom-pixels (pt2pt==pix2pix)
             int w = qRound( KoTextZoomHandler::layoutUnitPtToPt( c->customItem()->width ) );
             return applyZoom ? ( w * zh->zoom() / 100 ) : w;
         }
         else
             return 0;
    }
    int pixelww;
    int r = c->c.row();
    if( r < 0x06 || r > 0x1f )
    {
        // Use the cached font metrics from KoTextFormat
        if ( applyZoom )
        {
	    if ( r ) {
                pixelww = this->screenFontMetrics( zh ).width( displayedChar( c->c) );
	    } else {
                // Use the m_screenWidths[] array when possible, even faster
                Q_ASSERT( unicode < 256 );
		pixelww = d->m_screenWidths[ unicode ];
                // Not in cache yet -> calculate
                if ( pixelww == 0 ) {
                    pixelww = this->screenFontMetrics( zh ).width( displayedChar( c->c) );
                    Q_ASSERT( pixelww < 65535 );
                    d->m_screenWidths[ unicode ] = pixelww;
                }
	    }
        }
        else
            pixelww = this->refFontMetrics().width( displayedChar( c->c) );
    }
    else {
        // Here we have no choice, we need to create the format
        KoTextFormat tmpFormat( *this );  // make a copy
        tmpFormat.setPointSizeFloat( applyZoom ? screenPointSize( zh ) : refPointSize() );
        // complex text. We need some hacks to get the right metric here
        QString str;
        int pos = 0;
        if( i > 4 )
            pos = i - 4;
        int off = i - pos;
        int end = QMIN( parag->length(), i + 4 );
        while ( pos < end ) {
            str += displayedChar( parag->at(pos)->c);
            pos++;
        }
        pixelww = tmpFormat.width( str, off );
    }

#ifdef DEBUG_FORMATTER
    if ( applyZoom ) // ###
        qDebug( "\nKoTextFormatter::format: char=%s, LU-size=%d, LU-width=%d [equiv. to pix=%d] pixel-width=%d", // format=%s",
                QString(c->c).latin1(), this->font().pointSize(),
                ww, zh->layoutUnitToPixelX(ww), pixelww/*, this->key().latin1()*/ );
#endif
    return pixelww;
}

int KoTextFormat::height() const
{
    if ( d->m_refHeight < 0 )
    {
        // Calculate height using 100%-zoom font
        int h = refFontMetrics().height()+QABS(offsetFromBaseLine());
        if ( vAlign() == KoTextFormat::AlignSuperScript )
            h += refFontMetrics().height()/2;
        else if ( vAlign() == KoTextFormat::AlignSubScript )
            h += refFontMetrics().height()/6;
        //kdDebug(32500) << "KoTextFormat::height 100%-zoom font says h=" << h << " in LU:" << KoTextZoomHandler::ptToLayoutUnitPt(h) << endl;
        // Then scale to LU
        d->m_refHeight = qRound( KoTextZoomHandler::ptToLayoutUnitPt( h ) );
    }
    return d->m_refHeight;
}

QString KoTextFormat::displayedString( QString str )const
{
    if ( m_attributeFont== ATT_NONE)
        return str;
    else if ( m_attributeFont== ATT_UPPER)
        return str.upper();
    else if ( m_attributeFont== ATT_LOWER)
        return str.lower();
    else
    {
        kdDebug()<<" Error in AttributeStyle \n";
        return str;
    }
}

QChar KoTextFormat::displayedChar( QChar c )const
{
    if ( m_attributeFont== ATT_NONE)
        return c;
    else if ( m_attributeFont== ATT_UPPER)
        return c.upper();
    else if ( m_attributeFont== ATT_LOWER)
        return c.lower();
    else
    {
        kdDebug()<<" Error in AttributeStyle \n";
        return c;
    }
}

int KoTextFormat::ascent() const
{
    if ( d->m_refAscent < 0 )
    {
        // Calculate ascent using 100%-zoom font
        int h = refFontMetrics().ascent();
        if ( offsetFromBaseLine()>0 )
            h += offsetFromBaseLine();
        if ( vAlign() == KoTextFormat::AlignSuperScript )
            h += refFontMetrics().height()/2;
        // Then scale to LU
        d->m_refAscent = qRound( KoTextZoomHandler::ptToLayoutUnitPt( h ) );
    }
    return d->m_refAscent;
}

int KoTextFormat::descent() const
{
    if ( d->m_refDescent < 0 )
    {
        // Calculate descent using 100%-zoom font
        int h = refFontMetrics().descent();
        if ( offsetFromBaseLine()<0 )
            h -= offsetFromBaseLine();
        // Then scale to LU
        d->m_refDescent = qRound( KoTextZoomHandler::ptToLayoutUnitPt( h ) );
    }
    return d->m_refDescent;
}

int KoTextFormat::charWidthLU( const KoTextStringChar* c, const KoTextParag* parag, int i ) const
{
    return KoTextZoomHandler::ptToLayoutUnitPt( charWidth( 0L, false, c, parag, i ) );
}

int KoTextFormat::width( const QChar& ch ) const
{
    return KoTextZoomHandler::ptToLayoutUnitPt( refFontMetrics().width( ch ) );
}

//static
QString KoTextFormat::underlineStyleToString( KoTextFormat::UnderlineLineStyle _lineType )
{
    QString strLineType;
    switch ( _lineType )
    {
    case KoTextFormat::U_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::U_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::U_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::U_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::U_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;
}

QString KoTextFormat::strikeOutStyleToString( KoTextFormat::StrikeOutLineStyle _lineType )
{
    QString strLineType;
    switch ( _lineType )
    {
    case KoTextFormat::S_SOLID:
        strLineType ="solid";
        break;
    case KoTextFormat::S_DASH:
        strLineType ="dash";
        break;
    case KoTextFormat::S_DOT:
        strLineType ="dot";
        break;
    case KoTextFormat::S_DASH_DOT:
        strLineType="dashdot";
        break;
    case KoTextFormat::S_DASH_DOT_DOT:
        strLineType="dashdotdot";
        break;
    }
    return strLineType;
}

KoTextFormat::UnderlineLineStyle KoTextFormat::stringToUnderlineStyle( const QString & _str )
{
    if ( _str =="solid")
        return KoTextFormat::U_SOLID;
    else if ( _str =="dash" )
        return KoTextFormat::U_DASH;
    else if ( _str =="dot" )
        return KoTextFormat::U_DOT;
    else if ( _str =="dashdot")
        return KoTextFormat::U_DASH_DOT;
    else if ( _str=="dashdotdot")
        return KoTextFormat::U_DASH_DOT_DOT;
    else
        return KoTextFormat::U_SOLID;
}

KoTextFormat::StrikeOutLineStyle KoTextFormat::stringToStrikeOutStyle( const QString & _str )
{
    if ( _str =="solid")
        return KoTextFormat::S_SOLID;
    else if ( _str =="dash" )
        return KoTextFormat::S_DASH;
    else if ( _str =="dot" )
        return KoTextFormat::S_DOT;
    else if ( _str =="dashdot")
        return KoTextFormat::S_DASH_DOT;
    else if ( _str=="dashdotdot")
        return KoTextFormat::S_DASH_DOT_DOT;
    else
        return KoTextFormat::S_SOLID;
}

QString KoTextFormat::attributeFontToString( KoTextFormat::AttributeStyle _attr )
{
    if (_attr == KoTextFormat::ATT_NONE )
        return QString("none");
    else if ( _attr == KoTextFormat::ATT_UPPER )
        return QString("upper");
    else if ( _attr == KoTextFormat::ATT_LOWER )
        return QString("lower");
    else
        return QString("none");
}

KoTextFormat::AttributeStyle KoTextFormat::stringToAttributeFont( const QString & _str )
{
    if ( _str == "none" )
        return KoTextFormat::ATT_NONE;
    else if ( _str == "upper")
        return KoTextFormat::ATT_UPPER;
    else if ( _str == "lower")
        return KoTextFormat::ATT_LOWER;
    else
        return KoTextFormat::ATT_NONE;
}


void KoTextFormat::setSpellCheckingLanguage( const QString & _lang)
{
    if ( m_spellCheckLanguage == _lang )
        return;
    m_spellCheckLanguage = _lang;
    update();
}

#ifndef NDEBUG
void KoTextFormat::printDebug()
{
    QString col = color().isValid() ? color().name() : QString("(default)");
    qDebug( "format '%s' (%p): refcount: %d    realfont: %s color: %s",
            key().latin1(), (void*)this, ref, QFontInfo( font() ).family().latin1(), col.latin1() );
}
#endif

