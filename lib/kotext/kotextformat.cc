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
#include <klocale.h>
#include <assert.h>

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

KoTextFormat::KoTextFormat()
{
    linkColor = TRUE;
    ref = 0;
    missp = FALSE;
    ha = AlignNormal;
    collection = 0;
    //// kotext: WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
    d = new KoTextFormatPrivate;
    m_textUnderlineColor=QColor();
    m_underlineLine = U_NONE;
    m_strikeOutLine = S_NONE;
    m_underlineLineStyle = U_SOLID;
    m_strikeOutLineStyle = S_SOLID;
    m_language = KGlobal::locale()->language();
    ////
//#ifdef DEBUG_COLLECTION
//    qDebug("KoTextFormat simple ctor, no addRef, no generateKey ! %p",this);
//#endif
}

KoTextFormat::KoTextFormat( const QFont &f, const QColor &c, KoTextFormatCollection *parent )
    : fn( f ), col( c ), /*fm( QFontMetrics( f ) ),*/ linkColor( TRUE )
{
#ifdef DEBUG_COLLECTION
    qDebug("KoTextFormat with font & color & parent (%p), addRef. %p", parent, this);
#endif
    int pointSize;
    if ( f.pointSize() == -1 ) // font was set with a pixelsize, we need a pointsize!
        pointSize = (int)( ( (double)fn.pixelSize() * 72.0 ) / (double)QPaintDevice::x11AppDpiY() );
    else
        pointSize = f.pointSize();
    fn.setPointSize( pointSize );
    // WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
    ref = 0;
    collection = parent;
    //leftBearing = fm.minLeftBearing();
    //rightBearing = fm.minRightBearing();
    //hei = fm.height();
    //asc = fm.ascent();
    //dsc = fm.descent();
    missp = FALSE;
    ha = AlignNormal;
    //memset( widths, 0, 256 * sizeof( ushort ) );
    //// kotext
    d = new KoTextFormatPrivate;
    m_textUnderlineColor = QColor();
    m_underlineLine = U_NONE;
    m_strikeOutLine = S_NONE;
    m_underlineLineStyle = U_SOLID;
    m_strikeOutLineStyle = S_SOLID;
    //m_language = _language;
    m_language = KGlobal::locale()->language();
    ////
    generateKey();
    addRef();
    //updateStyleFlags();
}

KoTextFormat::KoTextFormat( const KoTextFormat &f )
{
    d = 0L;
    operator=( f );
}

KoTextFormat::~KoTextFormat()
{
    //// kotext addition
    // Removing a format that is in the collection is forbidden, in fact.
    // It should have been removed from the collection before being deleted.
#ifndef NDEBUG
    if ( parent() && parent()->defaultFormat() ) // not when destroying the collection
        assert( ! ( parent()->dict().find( key() ) == this ) );
        // (has to be the same pointer, not only the same key)
#endif
    delete d;
    ////
}

KoTextFormat& KoTextFormat::operator=( const KoTextFormat &f )
{
#ifdef DEBUG_COLLECTION
    qDebug("KoTextFormat::operator= %p (copying %p). Will addRef",this,&f);
#endif
    ref = 0;
    collection = 0; // f might be in the collection, but we are not
    fn = f.fn;
    col = f.col;
    //fm = f.fm;
    //leftBearing = f.leftBearing;
    //rightBearing = f.rightBearing;
    //memset( widths, 0, 256 * sizeof( ushort ) );
    //hei = f.hei;
    //asc = f.asc;
    //dsc = f.dsc;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    //// kotext addition
    delete d;
    d = new KoTextFormatPrivate;
    m_textBackColor=f.m_textBackColor;
    m_textUnderlineColor=f.m_textUnderlineColor;
    m_underlineLine = f.m_underlineLine;
    m_strikeOutLine = f.m_strikeOutLine;
    m_underlineLineStyle = f.m_underlineLineStyle;
    m_strikeOutLineStyle = f.m_strikeOutLineStyle;
    m_language = f.m_language;
    ////
    addRef();
    return *this;
}

void KoTextFormat::update()
{
    //qDebug("%p KoTextFormat::update %s %d",this, fn.family().latin1(),pointSize());
    fn.setStyleStrategy( QFont::ForceOutline );
    //fm = QFontMetrics( fn );
    //leftBearing = fm.minLeftBearing();
    //rightBearing = fm.minRightBearing();
    //hei = fm.height();
    //asc = fm.ascent();
    //dsc = fm.descent();
    //memset( widths, 0, 256 * sizeof( ushort ) );
    generateKey();
    //updateStyleFlags();
    //// kotext
    assert( d );
    d->clearCache(); // i.e. recalc at the next screenFont[Metrics]() call
    ////
}

void KoTextFormat::copyFormat( const KoTextFormat & nf, int flags )
{
    if ( flags & KoTextFormat::Bold )
	fn.setBold( nf.fn.bold() );
    if ( flags & KoTextFormat::Italic )
	fn.setItalic( nf.fn.italic() );
    if ( flags & KoTextFormat::Underline )
	fn.setUnderline( nf.fn.underline() );
    if ( flags & KoTextFormat::Family )
	fn.setFamily( nf.fn.family() );
    if ( flags & KoTextFormat::Size )
	fn.setPointSize( nf.fn.pointSize() );
    if ( flags & KoTextFormat::Color )
	col = nf.col;
    if ( flags & KoTextFormat::Misspelled )
	missp = nf.missp;
    if ( flags & KoTextFormat::VAlign )
	ha = nf.ha;
    ////// kotext addition
    if ( flags & KoTextFormat::StrikeOut )
    {
        setStrikeOutLineStyle( nf.strikeOutLineStyle() );
        setStrikeOutLineType (nf.strikeOutLineType());
    }
    if( flags & KoTextFormat::TextBackgroundColor)
        setTextBackgroundColor(nf.textBackgroundColor());
    if( flags & KoTextFormat::ExtendUnderLine)
    {
        setTextUnderlineColor(nf.textUnderlineColor());
        setUnderlineLineType (nf.underlineLineType());
        setUnderlineLineStyle (nf.underlineLineStyle());
    }
    if( flags & KoTextFormat::SpellCheckingLanguage)
        setSpellCheckingLanguage(nf.spellCheckingLanguage());

    //////
    update();
    //kdDebug(32500) << "KoTextFormat " << (void*)this << " copyFormat nf=" << (void*)&nf << " " << nf.key() << " flags=" << flags
    //        << " ==> result " << this << " " << key() << endl;
}

void KoTextFormat::setBold( bool b )
{
    if ( b == fn.bold() )
	return;
    fn.setBold( b );
    update();
}

void KoTextFormat::setMisspelled( bool b )
{
    if ( b == (bool)missp )
	return;
    missp = b;
    update();
}

void KoTextFormat::setVAlign( VerticalAlignment a )
{
    if ( a == ha )
	return;
    ha = a;
    update();
}

void KoTextFormat::setItalic( bool b )
{
    if ( b == fn.italic() )
	return;
    fn.setItalic( b );
    update();
}

void KoTextFormat::setUnderline( bool b )
{
    if ( b == fn.underline() )
	return;
    fn.setUnderline( b );
    update();
}

void KoTextFormat::setFamily( const QString &f )
{
    if ( f == fn.family() )
	return;
    fn.setFamily( f );
    update();
}

void KoTextFormat::setPointSize( int s )
{
    if ( s == fn.pointSize() )
	return;
    fn.setPointSize( s );
    update();
}

void KoTextFormat::setFont( const QFont &f )
{
    if ( f == fn && !k.isEmpty() )
	return;
    fn = f;
    update();
}

void KoTextFormat::setColor( const QColor &c )
{
    if ( c == col )
	return;
    col = c;
    update();
}

#if 0
int KoTextFormat::minLeftBearing() const
{
    if ( !painter || !painter->isActive() )
	return leftBearing;
    painter->setFont( fn );
    return painter->fontMetrics().minLeftBearing();
}

int KoTextFormat::minRightBearing() const
{
    if ( !painter || !painter->isActive() )
	return rightBearing;
    painter->setFont( fn );
    return painter->fontMetrics().minRightBearing();
}
#endif

void KoTextFormat::generateKey()
{
    k = fn.key();
    k += '/';
    if ( col.isValid() ) // just to shorten the key in the common case
        k += QString::number( (uint)col.rgb() );
    k += '/';
    k += QString::number( (int)isMisspelled() ); // a digit each, no need for '/'
    k += QString::number( (int)vAlign() );
    //// kotext addition
    k += '/';
    if (m_textBackColor.isValid())
        k += QString::number( (uint)m_textBackColor.rgb() );
    k += '/';
    if ( m_textUnderlineColor.isValid())
        k += QString::number( (uint)m_textUnderlineColor.rgb() );
    k += '/';
    k += QString::number( (int)m_underlineLine ); // a digit each, no need for '/'
    k += QString::number( (int)m_strikeOutLine );
    k += '/';
    k += QString::number( (int)m_underlineLineStyle );
    k += '/';
    k += QString::number( (int)m_strikeOutLineStyle);
    k += '/';
    k += m_language;
    ////
}

// This is used to create "simple formats", with font and color etc., but without
// advanced features. Doesn't matter, don't extend the args.
QString KoTextFormat::getKey( const QFont &fn, const QColor &col, bool misspelled, VerticalAlignment a )
{
    QString k = fn.key();
    k += '/';
    if ( col.isValid() ) // just to shorten the key in the common case
        k += QString::number( (uint)col.rgb() );
    k += '/';
    k += QString::number( (int)misspelled );
    k += QString::number( (int)a );
    //// kotext addition
    k += '/';
        // no background color
    k += '/';
        // no underline color
    k += '/';
    k += QString::number( (int)U_NONE );
    k += QString::number( (int)S_NONE ); // no double-underline in a "simple format"
    k += '/';
    k += QString::number( (int)U_SOLID );
    k += '/';
    k += QString::number( (int)S_SOLID ); // no double-underline in a "simple format"
    k += '/';
    //k += QString::null; // spellcheck language
    ////
    return k;
}

void KoTextFormat::addRef()
{
    ref++;
#ifdef DEBUG_COLLECTION
    if ( collection )
        qDebug( "  add ref of '%s' to %d (%p) (coll %p)", k.latin1(), ref, this, collection );
#endif
}

void KoTextFormat::removeRef()
{
    ref--;
    if ( !collection )
        return;
#ifdef DEBUG_COLLECTION
    qDebug( "  remove ref of '%s' to %d (%p) (coll %p)", k.latin1(), ref, this, collection );
#endif
    if ( ref == 0 )
        collection->remove( this );
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
    if ( pointSize() != format.pointSize() )
        flags |= KoTextFormat::Size;
    if ( color() != format.color() )
        flags |= KoTextFormat::Color;
    if ( vAlign() != format.vAlign() )
        flags |= KoTextFormat::VAlign;
    if ( strikeOutLineType() != format.strikeOutLineType()
        || underlineLineStyle() != format.underlineLineStyle())
        flags |= KoTextFormat::StrikeOut;
    if ( textBackgroundColor() != format.textBackgroundColor() )
        flags |= KoTextFormat::TextBackgroundColor;
    if ( spellCheckingLanguage() != format.spellCheckingLanguage() )
        flags |= KoTextFormat::SpellCheckingLanguage;
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
    // ## simplify (needs a change in KoZoomHandler)
    int pointSizeLU = KoTextZoomHandler::ptToLayoutUnitPt( pointSize() );
    if ( vAlign() != KoTextFormat::AlignNormal )
        pointSizeLU = ( ( pointSizeLU * 2 ) / 3 );

    return zh->layoutUnitToFontSize( pointSizeLU, false /* forPrint */ );
}

float KoTextFormat::refPointSize() const
{
    if ( vAlign() != KoTextFormat::AlignNormal )
        return ( (float)pointSize() * 2 ) / 3.0;
    else
        return pointSize();
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

QFont KoTextFormat::smallCapsFont( const KoZoomHandler* zh, bool applyZoom ) const
{
    QFont font = applyZoom ? screenFont( zh ) : refFont();
    QFontMetrics fm = refFontMetrics(); // only used for proportions, so applyZoom doesn't matter
    double pointSize = font.pointSize() * ((double)fm.boundingRect("x").height()/(double)fm.boundingRect("X").height());
    font.setPointSizeFloat( pointSize );
    return font;
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
                pixelww = this->screenFontMetrics( zh ).width( c->c );
	    } else {
                // Use the m_screenWidths[] array when possible, even faster
                Q_ASSERT( unicode < 256 );
		pixelww = d->m_screenWidths[ unicode ];
                // Not in cache yet -> calculate
                if ( pixelww == 0 ) {
                    pixelww = this->screenFontMetrics( zh ).width( c->c );
                    Q_ASSERT( pixelww < 65535 );
                    d->m_screenWidths[ unicode ] = pixelww;
                }
	    }
        }
        else
            pixelww = this->refFontMetrics().width( c->c );
    }
    else {
        // complex text. We need some hacks to get the right metric here
        const QFontMetrics& fontMetrics = applyZoom ? screenFontMetrics( zh ) : refFontMetrics();
        QString str;
        int pos = 0;
        if( i > 4 )
            pos = i - 4;
        int off = i - pos;
        int end = QMIN( parag->length(), i + 4 );
        while ( pos < end ) {
            str += parag->at(pos)->c;
            pos++;
        }
        pixelww = fontMetrics.charWidth( str, off );
    }

#ifdef DEBUG_FORMATTER
    if ( applyZoom ) // ###
        qDebug( "\nKoTextFormatter::format: char=%s, LU-size=%d, LU-width=%d [equiv. to pix=%d] pixel-width=%d", // format=%s",
                QString(c->c).latin1(), pointSize(),
                ww, zh->layoutUnitToPixelX(ww), pixelww/*, this->key().latin1()*/ );
#endif
    return pixelww;
}

int KoTextFormat::height() const
{
    if ( d->m_refHeight < 0 )
    {
        // Calculate height using 100%-zoom font
        int h = refFontMetrics().height();
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

int KoTextFormat::ascent() const
{
    if ( d->m_refAscent < 0 )
    {
        // Calculate ascent using 100%-zoom font
        int h = refFontMetrics().ascent();
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
        if ( vAlign() == KoTextFormat::AlignSuperScript )
            h += refFontMetrics().height()/2;
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

void KoTextFormat::setSpellCheckingLanguage( const QString & _lang)
{
    if ( m_language == _lang )
        return;
    m_language = _lang;
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

