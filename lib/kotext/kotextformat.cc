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
#include "kozoomhandler.h"
#include <kdebug.h>
#include <assert.h>

class KoTextFormatCollection::KoTextFormatCollectionPrivate
{
public:
    KoTextFormatCollectionPrivate() { m_cachedFormat = 0L; }
    KoTextFormat * m_cachedFormat;
    QFont m_cfont;
    QColor m_ccol;
};

KoTextFormatCollection::KoTextFormatCollection( const QFont & defaultFont )
    : QTextFormatCollection()
{
    d = new KoTextFormatCollectionPrivate;
    //kdDebug() << "KoTextFormatCollection::KoTextFormatCollection" << endl;
    //kdDebug() << "Deleting default format " << defaultFormat() << endl;
    delete defaultFormat();

    setDefaultFormat( new KoTextFormat( defaultFont, QColor(), 0L /* no coll, for no refcounting */ ) );
}

KoTextFormatCollection::~KoTextFormatCollection()
{
    delete d;
}

QTextFormat * KoTextFormatCollection::format( const QFont &fn, const QColor &c )
{
    if ( d->m_cachedFormat && d->m_cfont == fn && d->m_ccol == c ) {
        d->m_cachedFormat->addRef();
        return d->m_cachedFormat;
    }

    QString key = QTextFormat::getKey( fn, c, FALSE, QString::null, QString::null, QTextFormat::AlignNormal );
    kdDebug() << "format() textformat=" << this << " pointsizefloat=" << fn.pointSizeFloat() << endl;
    // SYNC any changes with generateKey below
    Q_ASSERT( !key.contains( '+' ) );
    key += '+';
    key += QString::number( (int)fn.strikeOut() );
    key += '/';
    key += QString::number( (int)(fn.pointSizeFloat() * 10) );
    // ######## Not needed in 3.0?
    //key += '/';
    //key += QString::number( (int)fn.charSet() );

    d->m_cachedFormat = static_cast<KoTextFormat *>( dict().find( key ) );
    d->m_cfont = fn;
    d->m_ccol = c;

    if ( d->m_cachedFormat ) {
        d->m_cachedFormat->addRef();
        return d->m_cachedFormat;
    }

    d->m_cachedFormat = static_cast<KoTextFormat *>( createFormat( fn, c ) );
    dict().insert( d->m_cachedFormat->key(), d->m_cachedFormat );
    return d->m_cachedFormat;
}

void KoTextFormatCollection::remove( QTextFormat *f )
{
    //kdDebug() << "KoTextFormatCollection::remove " << f << endl;
    if ( d->m_cachedFormat == f )
        d->m_cachedFormat = 0;
    QTextFormatCollection::remove( f );
}

bool KoTextFormatCollection::hasFormat( QTextFormat *f )
{
    return dict().find( f->key() ) == f; // has to be the same pointer, not only the same key
}

///

class KoTextFormat::KoTextFormatPrivate
{
public:
    KoTextFormatPrivate() : m_screenFont( 0L ), m_screenFontMetrics( 0L )
    {}
    // caching for speedup when formatting
    QFont* m_screenFont; // font to be used when painting (zoom-dependent)
    QFontMetrics* m_screenFontMetrics; // font metrics on screen (zoom-dependent)
};

KoTextFormat::KoTextFormat()
    : QTextFormat()
{
    d = new KoTextFormatPrivate;
    m_textBackColor=QColor();
    // ### The parent constructor didn't call our version of generateKey()
}

KoTextFormat::KoTextFormat( const KoTextFormat & fm )
    : QTextFormat( fm )
{
    d = new KoTextFormatPrivate;
    m_textBackColor=fm.textBackgroundColor();
    //kdDebug() << "KoTextFormat::KoTextFormat(copy of " << (void*)&fm << " " << fm.key() << ")"
    //          << " pointSizeFloat:" << fn.pointSizeFloat() << endl;
    // ### The parent constructor didn't call our version of generateKey()
}

KoTextFormat& KoTextFormat::operator=( const KoTextFormat &fm )
{
    QTextFormat::operator=( fm );
    m_textBackColor=fm.textBackgroundColor();
    return *this;
}

KoTextFormat::KoTextFormat( const QFont &f, const QColor &c, QTextFormatCollection * coll )
      : QTextFormat( f, c, coll )
{
    d = new KoTextFormatPrivate;
    generateKey();
}

KoTextFormat::~KoTextFormat()
{
    // Removing a format that is in the collection is forbidden, in fact.
    // It should have been removed from the collection before being deleted.
#ifndef NDEBUG
    if ( parent() && dynamic_cast<KoTextFormatCollection*>( parent() ) )
        assert( !static_cast<KoTextFormatCollection*>( parent() )->hasFormat( this ) );
#endif
    delete d->m_screenFontMetrics;
    delete d->m_screenFont;
    delete d;
}

void KoTextFormat::copyFormat( const QTextFormat & nf, int flags )
{
    QTextFormat::copyFormat( nf, flags );
    if ( flags & QTextFormat::Size )
        fn.setPointSizeFloat( nf.font().pointSizeFloat() );
    if ( flags & KoTextFormat::StrikeOut )
        fn.setStrikeOut( nf.font().strikeOut() );
    // ######## Not needed in 3.0?
    //if ( flags & KoTextFormat::CharSet )
    //fn.setCharSet( nf.font().charSet() );
    if( flags & KoTextFormat::TextBackgroundColor)
        setTextBackgroundColor(static_cast<const KoTextFormat *>(&nf)->textBackgroundColor());
    update();
    //kdDebug() << "KoTextFormat " << (void*)this << " copyFormat nf=" << (void*)&nf << " " << nf.key() << " flags=" << flags
    //        << " ==> result " << this << " " << key() << endl;
}

void KoTextFormat::setPointSizeFloat( float size )
{
    if ( fn.pointSizeFloat() == size )
        return;
    fn.setPointSizeFloat( size );
    update();
}

void KoTextFormat::setStrikeOut(bool b)
{
  if ( fn.strikeOut() == b )
        return;
    fn.setStrikeOut( b );
    update();
}

void KoTextFormat::setTextBackgroundColor(const QColor &_col)
{
    if(m_textBackColor==_col)
        return;
    m_textBackColor=_col;
    update();
}

void KoTextFormat::generateKey()
{
    QTextFormat::generateKey();
    QString k = key();
    Q_ASSERT( !k.contains( '+' ) );
    // SYNC any changes to the key format with ::format above
    k += '+';
    k += QString::number( (int)fn.strikeOut() );
    k += '/';
    k += QString::number( (int)(fn.pointSizeFloat() * 10) );
    k += '/';
    k += m_textBackColor.name();
    setKey( k );
    //kdDebug() << "generateKey textformat=" << this << " k=" << k << " pointsizefloat=" << fn.pointSizeFloat() << endl;

    // We also use this as a hook for update()
    delete d->m_screenFontMetrics;
    d->m_screenFontMetrics = 0L; // i.e. recalc at the next screenFontMetrics() call
    delete d->m_screenFont;
    d->m_screenFont = 0L; // i.e. recalc at the next screenFont() call
}

int KoTextFormat::compare( const KoTextFormat & format ) const
{
    int flags = 0;
    if ( fn.weight() != format.fn.weight() )
        flags |= QTextFormat::Bold;
    if ( fn.italic() != format.fn.italic() )
        flags |= QTextFormat::Italic;
    if ( fn.underline() != format.fn.underline() )
        flags |= QTextFormat::Underline;
    if ( fn.family() != format.fn.family() )
        flags |= QTextFormat::Family;
    if ( fn.pointSize() != format.fn.pointSize() )
        flags |= QTextFormat::Size;
    if ( color() != format.color() )
        flags |= QTextFormat::Color;
    // todo misspelled
    if ( vAlign() != format.vAlign() )
        flags |= QTextFormat::VAlign;

    if ( fn.strikeOut() != format.fn.strikeOut() )
        flags |= KoTextFormat::StrikeOut;
    if ( textBackgroundColor() != format.textBackgroundColor() )
        flags |= KoTextFormat::TextBackgroundColor;
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
    if ( vAlign() != QTextFormat::AlignNormal )
        pointSizeLU = ( ( pointSizeLU * 2 ) / 3 );

    return zh->layoutUnitToFontSize( pointSizeLU, false /* forPrint */ );
}

QFont KoTextFormat::screenFont( const KoZoomHandler* zh )
{
    float pointSize = screenPointSize( zh );
    // Compare if this is the size for which we cached the font metrics.
    // We have to do this very dynamically, because 2 views could be painting the same
    // stuff, with different zoom levels. So no absolute caching possible.
    if ( !d->m_screenFont || pointSize != d->m_screenFont->pointSizeFloat() )
    {
        delete d->m_screenFont;
        d->m_screenFont = new QFont( font() );
        d->m_screenFont->setPointSizeFloat( pointSize );
    }
    return *d->m_screenFont;
}

QFontMetrics KoTextFormat::screenFontMetrics( const KoZoomHandler* zh )
{
    float pointSize = screenPointSize( zh );
    if ( !d->m_screenFont )
        (void)screenFont( zh ); // we need it below, and this way it'll be ready for painting

    // Compare if this is the size for which we cached the font metrics.
    // We have to do this very dynamically, because 2 views could be painting the same
    // stuff, with different zoom levels. So no absolute caching possible.
    if ( !d->m_screenFontMetrics || pointSize != d->m_screenFont->pointSizeFloat() )
    {
        QFont f( font() );
        f.setPointSizeFloat( pointSize );
        delete d->m_screenFontMetrics;
        d->m_screenFontMetrics = new QFontMetrics( f );
    }
    return *d->m_screenFontMetrics;
}
