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

#ifndef kotextformat_h
#define kotextformat_h
#include "qrichtext_p.h"
using namespace Qt3;

/**
 * Our reimplementation of QTextFormat, to add setPixelSizeFloat(),
 * and storing StrikeOut and CharSet as part of the format.
 */
class KoTextFormat : public QTextFormat
{
public:
    KoTextFormat() : QTextFormat() {}
    virtual ~KoTextFormat() {}
    //KoTextFormat( const QStyleSheetItem *s );
    KoTextFormat( const QFont &f, const QColor &c, QTextFormatCollection * coll )
      : QTextFormat( f, c, coll ) { generateKey(); }
    KoTextFormat( const KoTextFormat &fm );
    //KoTextFormat& operator=( const KoTextFormat &fm );

    virtual void copyFormat( const QTextFormat & nf, int flags );

    /** Return a set of flags showing the differences between this and 'format' */
    int compare( const KoTextFormat & format ) const;

    /** Extending QTextFormat's enum */
    enum { StrikeOut = 256, CharSet = 512 };

    void setPointSizeFloat( float );
    float pointSizeFloat() const { return fn.pointSizeFloat(); }
    void setStrikeOut(bool);
    void setCharset( QFont::CharSet charset );

    /** Call this when a text color is set to 'invalid', meaning 'whatever the
     * default for the color scheme is' */
    static QColor defaultTextColor( QPainter * painter );

protected:
    virtual void generateKey();

    // NOTE: adding a member var here will make KoTextFormat bigger than QTextFormat,
    // which might break some casts somewhere. We'll need to do this, but carefully :)
    // (i.e. making sure that all formats are created as KoTextFormats)
};

/**
 * Our reimplementation of QTextFormatCollection, to provide KoTextFormats.
 */
class KoTextFormatCollection : public QTextFormatCollection
{
public:
    KoTextFormatCollection( const QFont & defaultFont );
    virtual ~KoTextFormatCollection() {}

    virtual QTextFormat *format( const QFont &f, const QColor &c );
    virtual void remove( QTextFormat *f );

    virtual QTextFormat *createFormat( const QTextFormat &fm ) { return new KoTextFormat( static_cast<const KoTextFormat &>(fm) ); }
    virtual QTextFormat *createFormat( const QFont &f, const QColor &c ) { return new KoTextFormat( f, c, this ); }

private:
    KoTextFormat * m_cachedFormat;
    QFont m_cfont;
    QColor m_ccol;

};

#endif
