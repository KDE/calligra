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
#include <kdebug.h>

KoTextFormatCollection::KoTextFormatCollection( const QFont & defaultFont )
    : QTextFormatCollection(), m_cachedFormat( 0L )
{
    //kdDebug() << "KoTextFormatCollection::KoTextFormatCollection" << endl;
    //kdDebug() << "Deleting default format " << defaultFormat() << endl;
    delete defaultFormat();

    setDefaultFormat( new KoTextFormat( defaultFont, QColor(), 0L /* no coll, for no refcounting */ ) );
}

QTextFormat * KoTextFormatCollection::format( const QFont &fn, const QColor &c )
{
    //kdDebug() << "KoTextFormatCollection::format font, color " << endl;
    if ( m_cachedFormat && m_cfont == fn && m_ccol == c ) {
        m_cachedFormat->addRef();
        return m_cachedFormat;
    }

    QString key = QTextFormat::getKey( fn, c, FALSE, QString::null, QString::null, QTextFormat::AlignNormal );
    //kdDebug() << "format() textformat=" << this << " pointsizefloat=" << fn.pointSizeFloat() << endl;
    // SYNC any changes with generateKey below
    ASSERT( !key.contains( '+' ) );
    key += '+';
    key += QString::number( (int)fn.strikeOut() );
    key += '/';
    key += QString::number( (int)(fn.pointSizeFloat() * 10) );
    key += '/';
    key += QString::number( (int)fn.charSet() );
    m_cachedFormat = static_cast<KoTextFormat *>( dict().find( key ) );
    m_cfont = fn;
    m_ccol = c;

    if ( m_cachedFormat ) {
        m_cachedFormat->addRef();
        return m_cachedFormat;
    }

    m_cachedFormat = static_cast<KoTextFormat *>( createFormat( fn, c ) );
    dict().insert( m_cachedFormat->key(), m_cachedFormat );
    return m_cachedFormat;
}

void KoTextFormatCollection::remove( QTextFormat *f )
{
    //kdDebug() << "KoTextFormatCollection::remove " << f << endl;
    if ( m_cachedFormat == f )
        m_cachedFormat = 0;
    QTextFormatCollection::remove( f );
}

///

KoTextFormat::KoTextFormat( const KoTextFormat & fm )
    : QTextFormat( fm )
{
    //kdDebug() << "KoTextFormat::KoTextFormat(copy of " << (void*)&fm << " " << fm.key() << ")"
    //          << " pointSizeFloat:" << fn.pointSizeFloat() << endl;
}

void KoTextFormat::copyFormat( const QTextFormat & nf, int flags )
{
    QTextFormat::copyFormat( nf, flags );
    if ( flags & QTextFormat::Size )
        fn.setPointSizeFloat( nf.font().pointSizeFloat() );
    if ( flags & KoTextFormat::StrikeOut )
        fn.setStrikeOut( nf.font().strikeOut() );
    if ( flags & KoTextFormat::CharSet )
        fn.setCharSet( nf.font().charSet() );
    update();
    //kdDebug() << "KoTextFormat " << (void*)this << " copyFormat nf=" << (void*)&nf
    //          << " " << nf.key() " flags=" << flags
    //          << " ==> result " << this << " " << key() << endl;
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

void KoTextFormat::setCharset( QFont::CharSet charset )
{
    if ( fn.charSet() == charset )
        return;
    fn.setCharSet( charset );
    update();
}

void KoTextFormat::generateKey()
{
    QTextFormat::generateKey();
    QString k = key();
    ASSERT( !k.contains( '+' ) );
    // SYNC any changes to the key format with ::format above
    k += '+';
    k += QString::number( (int)fn.strikeOut() );
    k += '/';
    k += QString::number( (int)(fn.pointSizeFloat() * 10) );
    k += '/';
    k += QString::number( (int)fn.charSet() );
    setKey( k );
    //kdDebug() << "generateKey textformat=" << this << " k=" << k << " pointsizefloat=" << fn.pointSizeFloat() << endl;
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
    if ( fn.charSet() != format.fn.charSet() )
        flags |= KoTextFormat::CharSet;

    return flags;
}

QColor KoTextFormat::defaultTextColor( QPainter * painter )
{
    if ( painter->device()->devType() == QInternal::Printer )
        return Qt::black;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Text );
}
