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

#include "kwformat.h"
#include <kdebug.h>
#include <kwdoc.h>
#if 0
#include <kglobal.h>
#include <kcharsets.h>
#endif

KWTextFormatCollection::KWTextFormatCollection( const QFont & defaultFont )
    : QTextFormatCollection(), m_cachedFormat( 0L )
{
    //kdDebug() << "KWTextFormatCollection::KWTextFormatCollection" << endl;
    //kdDebug() << "Deleting default format " << defaultFormat() << endl;
    delete defaultFormat();

    setDefaultFormat( new KWTextFormat( defaultFont, QColor(), 0L /* no coll, for no refcounting */ ) );
}

QTextFormat * KWTextFormatCollection::format( const QFont &f, const QColor &c )
{
    //kdDebug() << "KWTextFormatCollection::format font, color " << endl;
    if ( m_cachedFormat && m_cfont == f && m_ccol == c ) {
        m_cachedFormat->addRef();
        return m_cachedFormat;
    }

    QString key = QTextFormat::getKey( f, c, FALSE, QString::null, QString::null, QTextFormat::AlignNormal );
    //kdDebug() << "format() textformat=" << this << " pointsizefloat=" << f.pointSizeFloat() << endl;
     // SYNC any changes with generateKey below
    key += '+';
    key += QString::number( (int)f.strikeOut() );
    key += '/';
    key += QString::number( (int)(f.pointSizeFloat() * 10) );
    m_cachedFormat = static_cast<KWTextFormat *>( dict().find( key ) );
    m_cfont = f;
    m_ccol = c;

    if ( m_cachedFormat ) {
        m_cachedFormat->addRef();
        return m_cachedFormat;
    }

    m_cachedFormat = static_cast<KWTextFormat *>( createFormat( f, c ) );
    dict().insert( m_cachedFormat->key(), m_cachedFormat );
    return m_cachedFormat;
}

void KWTextFormatCollection::remove( QTextFormat *f )
{
    //kdDebug() << "KWTextFormatCollection::remove " << f << endl;
    if ( m_cachedFormat == f )
        m_cachedFormat = 0;
    QTextFormatCollection::remove( f );
}

///

KWTextFormat::KWTextFormat( const KWTextFormat & fm )
    : QTextFormat( fm )
{
    //kdDebug() << "KWTextFormat::KWTextFormat(copy of " << (void*)&fm << " " << fm.key() << ")"
    //          << " pointSizeFloat:" << fn.pointSizeFloat() << endl;
}

void KWTextFormat::copyFormat( const QTextFormat & nf, int flags )
{
    QTextFormat::copyFormat( nf, flags );
    if ( flags & QTextFormat::Size )
        fn.setPointSizeFloat( nf.font().pointSizeFloat() );
    if ( flags & KWTextFormat::StrikeOut )
        fn.setStrikeOut( nf.font().strikeOut() );
    update();
    //kdDebug() << "KWTextFormat " << (void*)this << " copyFormat nf=" << (void*)&nf << " " << nf.key()
    //          << " flags=" << flags << " ==> result " << this << " " << key() << endl;
}

void KWTextFormat::setPointSizeFloat( float size )
{
    if ( fn.pointSizeFloat() == size )
        return;
    fn.setPointSizeFloat( size );
    update();
}

void KWTextFormat::setStrikeOut(bool b)
{
  if ( fn.strikeOut() == b )
        return;
    fn.setStrikeOut( b );
    update();
}

void KWTextFormat::generateKey()
{
#if 0 // doesn't seem to help...
    // Small hack: we ask for the user's charset here, so that we do
    // it in all cases.
    fn.setCharSet( KGlobal::charsets()->charsetForLocale() );
#endif

    QTextFormat::generateKey();
    QString k = key();
    // SYNC any changes to the key format with ::format above
    k += '+';
    k += QString::number( (int)fn.strikeOut() );
    k += '/';
    k += QString::number( (int)(fn.pointSizeFloat() * 10) );
    setKey( k );
    //kdDebug() << "generateKey textformat=" << this << " k=" << k << " pointsizefloat=" << fn.pointSizeFloat() << endl;
}

int KWTextFormat::compare( const KWTextFormat & format ) const
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
        flags |= KWTextFormat::StrikeOut;

    return flags;
}
