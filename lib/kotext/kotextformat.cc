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

#include "kotextformat.h"
#include "korichtext.h" // for KoTextParag etc.
#include "kozoomhandler.h"
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <assert.h>
#include "kostyle.h"

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
    //linkColor = TRUE;
    ref = 0;
    missp = FALSE;
    va = AlignNormal;
    collection = 0;
    //// kotext: WYSIWYG works much much better with scalable fonts -> force it to be scalable
    fn.setStyleStrategy( QFont::ForceOutline );
    d = new KoTextFormatPrivate;
    m_textUnderlineColor=QColor();
    m_underlineType = U_NONE;
    m_strikeOutType = S_NONE;
    m_underlineStyle = U_SOLID;
    m_strikeOutStyle = S_SOLID;
    m_language = KGlobal::locale()->language();
    d->m_bHyphenation = false;
    d->m_underLineWidth = 1.0;
    d->m_shadowDistanceX = 0;
    d->m_shadowDistanceY = 0;
    d->m_relativeTextSize = 0.66;
    d->m_offsetFromBaseLine= 0;
    d->m_bWordByWord = false;
    m_attributeFont = ATT_NONE;
    ////
//#ifdef DEBUG_COLLECTION
//    kdDebug(32500) << "KoTextFormat simple ctor, no addRef, no generateKey ! " << this << endl;
//#endif
}

KoTextFormat::KoTextFormat( const QFont &f, const QColor &c, const QString &_language, bool hyphenation, double ulw, KoTextFormatCollection *parent )
    : fn( f ), col( c ) /*fm( QFontMetrics( f ) ),*/ //linkColor( TRUE )
{
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << "KoTextFormat with font & color & parent (" << parent << "), addRef. " << this << endl;
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
    va = AlignNormal;
    //// kotext
    d = new KoTextFormatPrivate;
    m_textUnderlineColor = QColor();
    m_underlineType = U_NONE;
    m_strikeOutType = S_NONE;
    m_underlineStyle = U_SOLID;
    m_strikeOutStyle = S_SOLID;
    m_language = _language;
    d->m_shadowDistanceX = 0;
    d->m_shadowDistanceY = 0;
    d->m_relativeTextSize= 0.66;
    d->m_offsetFromBaseLine = 0;
    d->m_bWordByWord = false;
    d->m_charStyle = 0L;
    d->m_bHyphenation = hyphenation;
    d->m_underLineWidth = ulw;
    m_attributeFont = ATT_NONE;
    ////
    generateKey();
    addRef();
}

KoTextFormat::KoTextFormat( const QFont &_font,
                            VerticalAlignment _valign,
                            const QColor & _color,
                            const QColor & _backGroundColor,
                            const QColor & _underlineColor,
                            KoTextFormat::UnderlineType _underlineType,
                            KoTextFormat::UnderlineStyle _underlineStyle,
                            KoTextFormat::StrikeOutType _strikeOutType,
                            KoTextFormat::StrikeOutStyle _strikeOutStyle,
                            KoTextFormat::AttributeStyle _fontAttribute,
                            const QString &_language,
                            double _relativeTextSize,
                            int _offsetFromBaseLine,
                            bool _wordByWord,
                            bool _hyphenation,
                            double _shadowDistanceX,
                            double _shadowDistanceY,
                            const QColor& _shadowColor )
{
    ref = 0;
    collection = 0;
    fn = _font;
    col = _color;
    missp = false;
    va = _valign;
    d = new KoTextFormatPrivate;
    m_textBackColor = _backGroundColor;
    m_textUnderlineColor = _underlineColor;
    m_underlineType = _underlineType;
    m_strikeOutType = _strikeOutType;
    m_underlineStyle = _underlineStyle;
    m_strikeOutStyle = _strikeOutStyle;
    m_language = _language;
    d->m_bHyphenation = _hyphenation;
    d->m_underLineWidth = 1.0;
    d->m_shadowDistanceX = _shadowDistanceX;
    d->m_shadowDistanceY = _shadowDistanceY;
    d->m_shadowColor = _shadowColor;
    d->m_relativeTextSize = _relativeTextSize;
    d->m_offsetFromBaseLine = _offsetFromBaseLine;
    d->m_bWordByWord = _wordByWord;
    m_attributeFont = _fontAttribute;
    d->m_charStyle = 0L;
    ////
    generateKey();
    addRef();
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
    kdDebug(32500) << "KoTextFormat::operator= " << this << " (copying " << &f << "). Will addRef" << endl;
#endif
    ref = 0;
    collection = 0; // f might be in the collection, but we are not
    fn = f.fn;
    col = f.col;
    //fm = f.fm;
    //leftBearing = f.leftBearing;
    //rightBearing = f.rightBearing;
    //hei = f.hei;
    //asc = f.asc;
    //dsc = f.dsc;
    missp = f.missp;
    va = f.va;
    k = f.k;
    //linkColor = f.linkColor;
    //// kotext addition
    delete d;
    d = new KoTextFormatPrivate;
    m_textBackColor=f.m_textBackColor;
    m_textUnderlineColor=f.m_textUnderlineColor;
    m_underlineType = f.m_underlineType;
    m_strikeOutType = f.m_strikeOutType;
    m_underlineStyle = f.m_underlineStyle;
    m_strikeOutStyle = f.m_strikeOutStyle;
    m_language = f.m_language;
    d->m_bHyphenation=f.d->m_bHyphenation;
    d->m_underLineWidth=f.d->m_underLineWidth;
    d->m_shadowDistanceX = f.d->m_shadowDistanceX;
    d->m_shadowDistanceY = f.d->m_shadowDistanceY;
    d->m_shadowColor = f.d->m_shadowColor;
    d->m_relativeTextSize = f.d->m_relativeTextSize;
    d->m_offsetFromBaseLine = f.d->m_offsetFromBaseLine;
    d->m_bWordByWord = f.d->m_bWordByWord;
    m_attributeFont = f.m_attributeFont;
    d->m_charStyle = 0L;
    ////
    addRef();
    return *this;
}

void KoTextFormat::update()
{
    //kdDebug(32500) << this << " KoTextFormat::update " << fn.family() << " " << pointSize() << endl;
    fn.setStyleStrategy( QFont::ForceOutline );
    //fm = QFontMetrics( fn );
    //leftBearing = fm.minLeftBearing();
    //rightBearing = fm.minRightBearing();
    //hei = fm.height();
    //asc = fm.ascent();
    //dsc = fm.descent();
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
    {
	va = nf.va;
        setRelativeTextSize( nf.relativeTextSize());
    }
    ////// kotext addition
    if ( flags & KoTextFormat::StrikeOut )
    {
        setStrikeOutStyle( nf.strikeOutStyle() );
        setStrikeOutType (nf.strikeOutType());
    }
    if( flags & KoTextFormat::TextBackgroundColor)
        setTextBackgroundColor(nf.textBackgroundColor());
    if( flags & KoTextFormat::ExtendUnderLine)
    {
        setTextUnderlineColor(nf.textUnderlineColor());
        setUnderlineType (nf.underlineType());
        setUnderlineStyle (nf.underlineStyle());
    }
    if( flags & KoTextFormat::Language)
        setLanguage(nf.language());
    if( flags & KoTextFormat::ShadowText)
        setShadow(nf.shadowDistanceX(), nf.shadowDistanceY(), nf.shadowColor());
    if( flags & KoTextFormat::OffsetFromBaseLine)
        setOffsetFromBaseLine(nf.offsetFromBaseLine());
    if( flags & KoTextFormat::WordByWord)
        setWordByWord(nf.wordByWord());
    if( flags & KoTextFormat::Attribute)
        setAttributeFont(nf.attributeFont());
    if( flags & KoTextFormat::Hyphenation )
        setHyphenation( nf.hyphenation());
    if( flags & KoTextFormat::UnderLineWidth )
        setUnderLineWidth( nf.underLineWidth());
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
    if ( a == va )
	return;
    va = a;
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

// ## Maybe we need a binary form for speed when NDEBUG, and to keep the
// ## readable form when !NDEBUG, like QFont does?
void KoTextFormat::generateKey()
{
    k = fn.key();
    k += '/';
    if ( col.isValid() ) // just to shorten the key in the common case
        k += QString::number( (uint)col.rgb() );
    k += '/';
    k += QString::number( (int)isMisspelled() ); // 1 digit, no need for '/'
    k += QString::number( (int)vAlign() );
    //// kotext addition
    k += '/';
    if (m_textBackColor.isValid())
        k += QString::number( (uint)m_textBackColor.rgb() );
    k += '/';
    if ( m_textUnderlineColor.isValid())
        k += QString::number( (uint)m_textUnderlineColor.rgb() );
    k += '/';
    k += QString::number( (int)m_underlineType ); // a digit each, no need for '/'
    k += QString::number( (int)m_strikeOutType );
    k += QString::number( (int)m_underlineStyle );
    k += QString::number( (int)m_strikeOutStyle );
    k += '/';
    k += m_language;
    k += '/';
    if ( d->m_shadowDistanceX != 0 || d->m_shadowDistanceY != 0 )
    {
        k += QString::number( d->m_shadowDistanceX );
        k += '/';
        k += QString::number( d->m_shadowDistanceY );
        k += '/';
        k += QString::number( (uint)d->m_shadowColor.rgb() );
    }
    k += '/';
    k += QString::number( d->m_relativeTextSize);
    k += '/';
    k += QString::number( d->m_offsetFromBaseLine);
    k += '/';
    k += QString::number( (int)d->m_bWordByWord); // boolean -> 1 digit -> no '/'
    k += QString::number( (int)m_attributeFont);
    k += '/';
    k += QString::number( (int)d->m_bHyphenation); // boolean -> 1 digit -> no '/'
    k += QString::number( (double)d->m_underLineWidth);
    ////
    // Keep in sync with method below
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
    k += QString::number( (int)U_SOLID );
    k += QString::number( (int)S_SOLID ); // no double-underline in a "simple format"
    k += '/';
    //k += QString::null; // spellcheck language
    k += '/';
      //no shadow
    k += '/';
    k += "0.66"; //relative text size
    k += '/';
    k += "0"; // no offset from base line
    k += '/';
    k += "0"; //no wordbyword attribute
    k += "0"; //no font attribute
    k += '/';
    k += "0"; //no hyphen
    k += "0"; //no ulw

    ////
    return k;
}

void KoTextFormat::addRef()
{
    ref++;
#ifdef DEBUG_COLLECTION
    if ( collection )
        kdDebug(32500) << "  add ref of '" << k << "' to " << ref << " (" << this << ") (coll " << collection << ")" << endl;
#endif
}

void KoTextFormat::removeRef()
{
    ref--;
    if ( !collection )
        return;
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << "  remove ref of '" << k << "' to " << ref << " (" << this << ") (coll " << collection << ")" << endl;
#endif
    if ( ref == 0 )
        collection->remove( this );
}

void KoTextFormat::setStrikeOutType (StrikeOutType _type)
{
    if ( m_strikeOutType == _type )
        return;
    m_strikeOutType = _type;
    update();
}

void KoTextFormat::setUnderlineType (UnderlineType _type)
{
    if ( m_underlineType == _type )
        return;
    m_underlineType = _type;
    update();
}

void KoTextFormat::setUnderlineStyle (UnderlineStyle _type)
{
    if ( m_underlineStyle == _type )
        return;
    m_underlineStyle = _type;
    update();
}

void KoTextFormat::setStrikeOutStyle( StrikeOutStyle _type )
{
    if ( m_strikeOutStyle == _type )
        return;
    m_strikeOutStyle = _type;
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

void KoTextFormat::setShadow( double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor )
{
    if ( d->m_shadowDistanceX == shadowDistanceX &&
         d->m_shadowDistanceY == shadowDistanceY &&
         d->m_shadowColor == shadowColor )
        return;
    d->m_shadowDistanceX = shadowDistanceX;
    d->m_shadowDistanceY = shadowDistanceY;
    d->m_shadowColor = shadowColor;
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
         underlineType()!= format.underlineType() ||
         underlineStyle() != format.underlineStyle())
        flags |= KoTextFormat::ExtendUnderLine;
    if ( fn.family() != format.fn.family() )
        flags |= KoTextFormat::Family;
    if ( pointSize() != format.pointSize() )
        flags |= KoTextFormat::Size;
    if ( color() != format.color() )
        flags |= KoTextFormat::Color;
    if ( vAlign() != format.vAlign() ||
        relativeTextSize() != format.relativeTextSize())
        flags |= KoTextFormat::VAlign;
    if ( strikeOutType() != format.strikeOutType()
        || underlineStyle() != format.underlineStyle())
        flags |= KoTextFormat::StrikeOut;
    if ( textBackgroundColor() != format.textBackgroundColor() )
        flags |= KoTextFormat::TextBackgroundColor;
    if ( language() != format.language() )
        flags |= KoTextFormat::Language;
    if ( d->m_shadowDistanceX != format.shadowDistanceX()
         || d->m_shadowDistanceY != format.shadowDistanceY()
         || d->m_shadowColor != format.shadowColor() )
        flags |= KoTextFormat::ShadowText;
    if ( offsetFromBaseLine() != format.offsetFromBaseLine() )
        flags |= KoTextFormat::OffsetFromBaseLine;
    if ( wordByWord() != format.wordByWord() )
        flags |= KoTextFormat::WordByWord;
    if ( attributeFont() != format.attributeFont() )
        flags |= KoTextFormat::Attribute;
    if( hyphenation() != format.hyphenation() )
        flags |= KoTextFormat::Hyphenation;
    if( underLineWidth() != format.underLineWidth() )
        flags |= KoTextFormat::UnderLineWidth;
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
        pointSizeLU = (int)( pointSizeLU *relativeTextSize() );
    return zh->layoutUnitToFontSize( pointSizeLU, false /* forPrint */ );
}

float KoTextFormat::refPointSize() const
{
    if ( vAlign() != KoTextFormat::AlignNormal )
        return (float)pointSize() * relativeTextSize();
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
        delete d->m_refFontMetrics;
        d->m_refFontMetrics = 0;
        //kdDebug(32500) << "KoTextFormat::refFont created new font with size " << pointSize << endl;
    }
    return *d->m_refFont;
}

QFont KoTextFormat::screenFont( const KoZoomHandler* zh ) const
{
    float pointSize = screenPointSize( zh );
    //kdDebug(32500) << "KoTextFormat::screenFont pointSize=" << pointSize << endl;
    // Compare if this is the size for which we cached the font metrics.
    // We have to do this very dynamically, because 2 views could be painting the same
    // stuff, with different zoom levels. So no absolute caching possible.
    /*if ( d->m_screenFont )
      kdDebug(32500) << " d->m_screenFont->pointSizeFloat()=" << d->m_screenFont->pointSizeFloat() << endl;*/
    if ( !d->m_screenFont || kAbs( pointSize - d->m_screenFont->pointSizeFloat() ) > 1E-4 )
    {
        delete d->m_screenFont;
        d->m_screenFont = new QFont( font() );
        d->m_screenFont->setPointSizeFloat( pointSize );
        delete d->m_screenFontMetrics;
        d->m_screenFontMetrics = 0;
        //kdDebug(32500) << "KoTextFormat::screenFont created new font with size " << pointSize << endl;
    }
    return *d->m_screenFont;
}

const QFontMetrics& KoTextFormat::screenFontMetrics( const KoZoomHandler* zh ) const
{
    QFont f = screenFont(zh); // don't move inside the if!

    if ( !d->m_screenFontMetrics ) // not calculated, or invalidated by screenFont above
    {
        //kdDebug(32500) << this << " KoTextFormat::screenFontMetrics pointSize=" << pointSize << " d->m_screenFont->pointSizeFloat()=" << d->m_screenFont->pointSizeFloat() << endl;
        d->m_screenFontMetrics = new QFontMetrics( f );
        //kdDebug(32500) << "KoTextFormat::screenFontMetrics created new metrics with size " << pointSize << "   height:" << d->m_screenFontMetrics->height() << endl;
    }
    return *d->m_screenFontMetrics;
}

const QFontMetrics& KoTextFormat::refFontMetrics() const
{
    QFont f = refFont();

    if ( !d->m_refFontMetrics )
    {
        //kdDebug(32500) << this << " KoTextFormat::refFontMetrics pointSize=" << pointSize << " d->m_refFont->pointSizeFloat()=" << d->m_refFont->pointSizeFloat() << endl;
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
    if ( !c->charStop || unicode == 0xad || unicode == 0x2028 )
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
    if( /*r < 0x06 || r > 0x1f*/ r < 0x06 || (r > 0x1f && !(r > 0xd7 && r < 0xe0)) )
    {
        // Small caps -> we can't use the cached font metrics from KoTextFormat
        if ( attributeFont() == KoTextFormat::ATT_SMALL_CAPS && c->c.upper() != c->c )
        {
            pixelww = QFontMetrics( smallCapsFont( zh, applyZoom ) ).width( displayedChar( c->c ) );
        }
        else
        // Use the cached font metrics from KoTextFormat
        if ( applyZoom )
        {
	    if ( r ) {
                pixelww = this->screenFontMetrics( zh ).width( displayedChar( c->c ) );
	    } else {
                // Use the m_screenWidths[] array when possible, even faster
                Q_ASSERT( unicode < 256 );
		pixelww = d->m_screenWidths[ unicode ];
                // Not in cache yet -> calculate
                if ( pixelww == 0 ) {
                    pixelww = this->screenFontMetrics( zh ).width( displayedChar( c->c ) );
                    Q_ASSERT( pixelww < 65535 );
                    d->m_screenWidths[ unicode ] = pixelww;
                }
	    }
        }
        else {
            pixelww = this->refFontMetrics().width( displayedChar( c->c ) );
	}
    }
    else {
        // Complex text. We need some hacks to get the right metric here
        bool smallCaps = ( attributeFont() == KoTextFormat::ATT_SMALL_CAPS && c->c.upper() != c->c );
        const QFontMetrics& fontMetrics = smallCaps ? smallCapsFont( zh, applyZoom ) : applyZoom ? screenFontMetrics( zh ) : refFontMetrics();
        QString str;
        int pos = 0;
        if( i > 8 )
            pos = i - 8;
        int off = i - pos;
        int end = QMIN( parag->length(), i + 8 );
        while ( pos < end ) {
            str += displayedChar( parag->at(pos)->c );
            pos++;
        }
        pixelww = fontMetrics.charWidth( str, off );
    }

#if 0
    // Add room for the shadow - hmm, this is wrong. A word with a shadow
    // doesn't need to space its chars so that the shadow never runs into
    // the next char. The usual effect is that it DOES run into other chars.
    if ( d->m_shadowDistanceX != 0 )
    {
        // pt to pixel conversion
        int shadowpix = (int)(POINT_TO_INCH( static_cast<double>( QPaintDevice::x11AppDpiX() ) ) * QABS( d->m_shadowDistanceX ) );
        //kdDebug(32500) << "d->m_shadowDistanceX=" << d->m_shadowDistanceX << " -> shadowpix=" << shadowpix
        //      << ( applyZoom ? " and applying zoom " : " (100% zoom) " )
        //      << " -> adding " << ( applyZoom ? (  shadowpix * zh->zoom() / 100 ) : shadowpix ) << endl;
        pixelww += applyZoom ? ( shadowpix * zh->zoom() / 100 ) : shadowpix;
    }
#endif

#if 0
        kdDebug(32500) << "KoTextFormat::charWidth: char=" << QString(c->c) << " format=" << key()
                       << ", applyZoom=" << applyZoom << " pixel-width=" << pixelww << endl;
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

        // Add room for the shadow
        if ( d->m_shadowDistanceY != 0 ) {
            // pt -> pixel (at 100% zoom)
            h += (int)(POINT_TO_INCH( static_cast<double>( QPaintDevice::x11AppDpiY() ) ) * QABS( d->m_shadowDistanceY ) );
        }

        //kdDebug(32500) << "KoTextFormat::height 100%-zoom font says h=" << h << " in LU:" << KoTextZoomHandler::ptToLayoutUnitPt(h) << endl;
        // Then scale to LU
        d->m_refHeight = qRound( KoTextZoomHandler::ptToLayoutUnitPt( h ) );
    }
    return d->m_refHeight;
}

int KoTextFormat::offsetX() const // in LU pixels
{
    int off = 0;
#if 0
    // Shadow on left -> character is moved to the right
    // Wrong if next char has no shadow (they'll run into each other)
    // Somehow we should only do this if x == 0 (in the formatter)
    if ( d->m_shadowDistanceX < 0 )
    {
        double lupt = KoTextZoomHandler::ptToLayoutUnitPt( QABS( d->m_shadowDistanceX ) );
        off += (int)(POINT_TO_INCH( static_cast<double>( QPaintDevice::x11AppDpiX() ) ) * lupt );
    }
#endif
    return off;
}

int KoTextFormat::offsetY() const // in LU pixels
{
    int off = 0;
#if 0
    // Shadow on top -> character is moved down
    if ( d->m_shadowDistanceY < 0 )
    {
        double lupt = KoTextZoomHandler::ptToLayoutUnitPt( QABS( d->m_shadowDistanceY ) );
        off += (int)(POINT_TO_INCH( static_cast<double>( QPaintDevice::x11AppDpiY() ) ) * lupt );
    }
#endif
    return off;
}

QString KoTextFormat::displayedString( const QString& str )const
{
    switch ( m_attributeFont ) {
    case ATT_NONE:
        return str;
    case ATT_UPPER:
    case ATT_SMALL_CAPS:
        return str.upper();
    case ATT_LOWER:
        return str.lower();
    default:
        kdDebug(32500)<<" Error in AttributeStyle \n";
        return str;
    }
}

QChar KoTextFormat::displayedChar( QChar c )const
{
    if ( c.unicode() == 0xa0 ) // nbsp
        return ' ';
    switch ( m_attributeFont ) {
    case ATT_NONE:
        return c;
    case ATT_SMALL_CAPS:
    case ATT_UPPER:
        return c.upper();
    case ATT_LOWER:
        return c.lower();
    default:
        kdDebug(32500)<<" Error in AttributeStyle \n";
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
        //d->m_refAscent += offsetY();
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
        //d->m_refDescent += offsetY();
    }
    return d->m_refDescent;
}

int KoTextFormat::charWidthLU( const KoTextStringChar* c, const KoTextParag* parag, int i ) const
{
    // Hmm, we add precision to the least precise one!
    // TODO: We should instead implement it here in LU, and let charWidth call it...
   return KoTextZoomHandler::ptToLayoutUnitPt( charWidth( 0L, false, c, parag, i ) );
}

int KoTextFormat::width( const QChar& ch ) const
{
    // Warning this doesn't take into account the shadow
    return KoTextZoomHandler::ptToLayoutUnitPt( refFontMetrics().width( ch ) );
}

void KoTextFormat::applyCharStyle( KoCharStyle *_style )
{
    d->m_charStyle = _style;
}

KoCharStyle *KoTextFormat::style() const
{
    return d->m_charStyle;
}

QString KoTextFormat::shadowAsCss(  double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor )
{
    // http://www.w3.org/TR/REC-CSS2/text.html#text-shadow-props
    // none | [<color> || <length (h)> <length (v)> <length (blur radius, not used here)>] ...
    // => none or color length length
    if ( shadowDistanceX != 0 || shadowDistanceY != 0 )
    {
        QString css = shadowColor.name() + " ";
        css += QString::number(shadowDistanceX) + "pt ";
        css += QString::number(shadowDistanceY) + "pt";
        return css;
    }
    return "none";
}

QString KoTextFormat::shadowAsCss() const
{
    return shadowAsCss( d->m_shadowDistanceX, d->m_shadowDistanceY, d->m_shadowColor );
}

void KoTextFormat::parseShadowFromCss( const QString& _css )
{
    QString css = _css.simplifyWhiteSpace();
    if ( css.isEmpty() || css == "none" )
    {
        d->m_shadowDistanceX = 0;
        d->m_shadowDistanceY = 0;
        d->m_shadowColor = QColor();
    } else
    {
        QStringList tokens = QStringList::split(' ', css);
        if ( tokens.isEmpty() ) {
            kdWarning(32500) << "Parse error in text-shadow: " << css << endl;
            return;
        }
        // Check which token looks like a colour
        QColor col( tokens.first() );
        if ( col.isValid() )
            tokens.pop_front();
        else if ( tokens.count() > 1 )
        {
            col.setNamedColor( tokens.last() );
            if ( col.isValid() )
                tokens.pop_back();
        }
        d->m_shadowColor = col; // whether valid or not
        // Parse x distance
        if ( !tokens.isEmpty() ) {
            d->m_shadowDistanceX = KoUnit::parseValue( tokens.first() );
            tokens.pop_front();
        }
        // Parse y distance
        if ( !tokens.isEmpty() ) {
            d->m_shadowDistanceY = KoUnit::parseValue( tokens.first() );
            tokens.pop_front();
        }
        // We ignore whatever else is in the string (e.g. blur radius, other shadows)
    }
    update();
}

QColor KoTextFormat::shadowColor() const
{
    if ( d->m_shadowColor.isValid() )
        return d->m_shadowColor;
    else // CSS says "[If] no color has been specified, the shadow will have the same color as the [text] itself"
        return col;
}

int KoTextFormat::shadowX( KoZoomHandler *zh ) const
{
    return zh->zoomItX( d->m_shadowDistanceX );
}

int KoTextFormat::shadowY( KoZoomHandler *zh ) const
{
    return zh->zoomItY( d->m_shadowDistanceY );
}

//static
QString KoTextFormat::underlineStyleToString( KoTextFormat::UnderlineStyle _lineType )
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

QString KoTextFormat::strikeOutStyleToString( KoTextFormat::StrikeOutStyle _lineType )
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

KoTextFormat::UnderlineStyle KoTextFormat::stringToUnderlineStyle( const QString & _str )
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

KoTextFormat::StrikeOutStyle KoTextFormat::stringToStrikeOutStyle( const QString & _str )
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
        return QString("uppercase");
    else if ( _attr == KoTextFormat::ATT_LOWER )
        return QString("lowercase");
    else if ( _attr == KoTextFormat::ATT_SMALL_CAPS )
        return QString("smallcaps");
    else
        return QString("none");
}

KoTextFormat::AttributeStyle KoTextFormat::stringToAttributeFont( const QString & _str )
{
    if ( _str == "none" )
        return KoTextFormat::ATT_NONE;
    else if ( _str == "uppercase")
        return KoTextFormat::ATT_UPPER;
    else if ( _str == "lowercase")
        return KoTextFormat::ATT_LOWER;
    else if ( _str == "smallcaps" )
        return KoTextFormat::ATT_SMALL_CAPS;
    else
        return KoTextFormat::ATT_NONE;
}


void KoTextFormat::setHyphenation( bool b )
{
    if ( d->m_bHyphenation == b )
        return;
    d->m_bHyphenation = b;
    update();

}

void KoTextFormat::setUnderLineWidth( double ulw )
{
    if ( d->m_underLineWidth == ulw )
        return;
    d->m_underLineWidth = ulw;
    update();

}

void KoTextFormat::setLanguage( const QString & _lang)
{
    if ( m_language == _lang )
        return;
    m_language = _lang;
    update();
}

QStringList KoTextFormat::underlineTypeList()
{
    QStringList lst;
    lst <<i18n("Without");
    lst <<i18n("Single");
    lst <<i18n("Simple Bold");
    lst <<i18n("Double");
    lst <<i18n("Wave");
    return lst;
}

QStringList KoTextFormat::strikeOutTypeList()
{
    QStringList lst;
    lst <<i18n("Without");
    lst <<i18n("Single");
    lst <<i18n("Simple Bold");
    lst <<i18n("Double");
    return lst;
}

QStringList KoTextFormat::fontAttributeList()
{
    QStringList lst;
    lst <<i18n("Normal");
    lst <<i18n("Uppercase");
    lst <<i18n("Lowercase");
    lst <<i18n("Small Caps");
    return lst;
}

QStringList KoTextFormat::underlineStyleList()
{
    QStringList lst;
    lst <<i18n("Solid Line");
    lst <<i18n("Dash Line");
    lst <<i18n("Dot Line");
    lst <<i18n("Dash Dot Line");
    lst <<i18n("Dash Dot Dot Line");
    return lst;
}

QStringList KoTextFormat::strikeOutStyleList()
{
    QStringList lst;
    lst <<i18n("Solid Line");
    lst <<i18n("Dash Line");
    lst <<i18n("Dot Line");
    lst <<i18n("Dash Dot Line");
    lst <<i18n("Dash Dot Dot Line");
    return lst;
}


#ifndef NDEBUG
void KoTextFormat::printDebug()
{
    QString col = color().isValid() ? color().name() : QString("(default)");
    kdDebug(32500) << "format '" << key() << "' (" << (void*)this << "):"
                   << " refcount: " << ref
                   << " realfont: " << QFontInfo( font() ).family()
                   << " color: " << col << " shadow=" << shadowAsCss() << endl;
}
#endif

////////////////

KoTextFormatCollection::KoTextFormatCollection()
    : cKey( 307 )//, sheet( 0 )
{
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << "KoTextFormatCollection::KoTextFormatCollection " << this << endl;
#endif
    defFormat = new KoTextFormat( QApplication::font(), QColor(), KGlobal::locale()->language(), false, 1.0 ); //// kotext: need to use default QColor here
    lastFormat = cres = 0;
    cflags = -1;
    cKey.setAutoDelete( TRUE );
    cachedFormat = 0;
}

KoTextFormatCollection::KoTextFormatCollection( const QFont& defaultFont, const QColor& defaultColor, const QString & defaultLanguage, bool hyphen, double ulw )
    : cKey( 307 )//, sheet( 0 )
{
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << "KoTextFormatCollection::KoTextFormatCollection " << this << endl;
#endif
    defFormat = new KoTextFormat( defaultFont, defaultColor, defaultLanguage, hyphen, ulw );
    lastFormat = cres = 0;
    cflags = -1;
    cKey.setAutoDelete( TRUE );
    cachedFormat = 0;
}

KoTextFormatCollection::~KoTextFormatCollection()
{
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << "KoTextFormatCollection::~KoTextFormatCollection " << this << endl;
#endif
    delete defFormat;
    defFormat = 0;
}

KoTextFormat *KoTextFormatCollection::format( const KoTextFormat *f )
{
    if ( f->parent() == this || f == defFormat ) {
#ifdef DEBUG_COLLECTION
        kdDebug(32500) << " format(f) need '" << f->key() << "', best case!" << endl;
#endif
	lastFormat = const_cast<KoTextFormat*>(f);
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f == lastFormat || ( lastFormat && f->key() == lastFormat->key() ) ) {
#ifdef DEBUG_COLLECTION
        kdDebug(32500) << " format(f) need '" << f->key() << "', good case!" << endl;
#endif
	lastFormat->addRef();
	return lastFormat;
    }

#if 0 // #### disabled, because if this format is not in the
 // formatcollection, it doesn't get the painter through
 // KoTextFormatCollection::setPainter() which breaks printing on
 // windows
    if ( f->isAnchor() ) {
	lastFormat = createFormat( *f );
	lastFormat->collection = 0;
	return lastFormat;
    }
#endif

    KoTextFormat *fm = cKey.find( f->key() );
    if ( fm ) {
#ifdef DEBUG_COLLECTION
        kdDebug(32500) << " format(f) need '" << f->key() << "', normal case!" << endl;
#endif
	lastFormat = fm;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f->key() == defFormat->key() )
	return defFormat;

#ifdef DEBUG_COLLECTION
    kdDebug(32500) << " format(f) need '" << f->key() << "', worst case!" << endl;
#endif
    lastFormat = createFormat( *f );
    lastFormat->collection = this;
    cKey.insert( lastFormat->key(), lastFormat );
    Q_ASSERT( f->key() == lastFormat->key() );
    return lastFormat;
}

KoTextFormat *KoTextFormatCollection::format( const KoTextFormat *of, const KoTextFormat *nf, int flags )
{
    if ( cres && kof == of->key() && knf == nf->key() && cflags == flags ) {
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << " format(of,nf,flags) mix of '" << of->key() << "' and '" << nf->key() << "', best case!" << endl;
#endif
	cres->addRef();
	return cres;
    }

#ifdef DEBUG_COLLECTION
    kdDebug(32500) << " format(of,nf," << flags << ") calling createFormat(of=" << of << " " << of->key() << ")" << endl;
#endif
    cres = createFormat( *of );
    kof = of->key();
    knf = nf->key();
    cflags = flags;

#ifdef DEBUG_COLLECTION
    kdDebug(32500) << " format(of,nf," << flags << ") calling copyFormat(nf=" << nf << " " << nf->key() << ")" << endl;
#endif
    cres->copyFormat( *nf, flags );

    KoTextFormat *fm = cKey.find( cres->key() );
    if ( !fm ) {
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << " format(of,nf,flags) mix of '" << of->key() << "' and '" << nf->key() << ", worst case!" << endl;
#endif
	cres->collection = this;
	cKey.insert( cres->key(), cres );
    } else {
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << " format(of,nf,flags) mix of '" << of->key() << "' and '" << nf->key() << ", good case!" << endl;
#endif
	delete cres;
	cres = fm;
	cres->addRef();
    }

    return cres;
}

KoTextFormat *KoTextFormatCollection::format( const QFont &f, const QColor &c, const QString & language, bool hyphen, double ulw )
{
    if ( cachedFormat && cfont == f && ccol == c ) {
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << " format of font and col '" << cachedFormat->key() << "' - best case" << endl;
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    QString key = KoTextFormat::getKey( f, c, FALSE, KoTextFormat::AlignNormal );
    cachedFormat = cKey.find( key );
    cfont = f;
    ccol = c;

    if ( cachedFormat ) {
#ifdef DEBUG_COLLECTION
	kdDebug(32500) << " format of font and col '" << cachedFormat->key() << "' - good case" << endl;
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    if ( key == defFormat->key() )
	return defFormat;

    cachedFormat = createFormat( f, c,language, hyphen, ulw );
    cachedFormat->collection = this;
    cKey.insert( cachedFormat->key(), cachedFormat );
    if ( cachedFormat->key() != key )
	kdWarning() << "ASSERT: keys for format not identical: '" << cachedFormat->key() << " '" << key << "'" << endl;
#ifdef DEBUG_COLLECTION
    kdDebug(32500) << " format of font and col '" << cachedFormat->key() << "' - worst case" << endl;
#endif
    return cachedFormat;
}

void KoTextFormatCollection::remove( KoTextFormat *f )
{
    if ( lastFormat == f )
	lastFormat = 0;
    if ( cres == f )
	cres = 0;
    if ( cachedFormat == f )
	cachedFormat = 0;
    cKey.remove( f->key() );
}

#if 0
void KoTextFormatCollection::setPainter( QPainter *p )
{
    QDictIterator<KoTextFormat> it( cKey );
    KoTextFormat *f;
    while ( ( f = it.current() ) ) {
	++it;
	f->setPainter( p );
    }
}
#endif

#ifndef NDEBUG
void KoTextFormatCollection::debug()
{
    kdDebug(32500) << "------------ KoTextFormatCollection: debug --------------- BEGIN" << endl;
    kdDebug(32500) << "Default Format: '" << defFormat->key() << "' (" << (void*)defFormat << "): realfont: " << QFontInfo( defFormat->font() ).family() << endl;
    QDictIterator<KoTextFormat> it( cKey );
    for ( ; it.current(); ++it ) {
         Q_ASSERT(it.currentKey() == it.current()->key());
         if(it.currentKey() != it.current()->key())
             kdDebug(32500) << "**** MISMATCH key=" << it.currentKey() << " (see line below for format)" << endl;
	 it.current()->printDebug();
    }
    kdDebug(32500) << "------------ KoTextFormatCollection: debug --------------- END" << endl;
}

#endif
