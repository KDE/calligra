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
class KoZoomHandler;

/**
 * Our reimplementation of QTextFormat, to add setPixelSizeFloat(),
 * and storing StrikeOut and CharSet as part of the format.
 */
class KoTextFormat : public QTextFormat
{
public:
    KoTextFormat();
    virtual ~KoTextFormat();
    //KoTextFormat( const QStyleSheetItem *s );
    KoTextFormat( const QFont &f, const QColor &c, QTextFormatCollection * coll );
    KoTextFormat( const KoTextFormat &fm );
    KoTextFormat& operator=( const KoTextFormat &fm );

    virtual void copyFormat( const QTextFormat & nf, int flags );

    /** Return a set of flags showing the differences between this and 'format' */
    int compare( const KoTextFormat & format ) const;

    /** Extending QTextFormat's enum */
    enum { StrikeOut = 512 , CharSet = 1024, TextBackgroundColor = 2048 };

    void setPointSizeFloat( float );
    float pointSizeFloat() const { return fn.pointSizeFloat(); }
    void setStrikeOut(bool);
    // ####### Not needed with 3.0? (Werner)
    //void setCharset( QFont::CharSet charset );

    /** Call this when a text color is set to 'invalid', meaning 'whatever the
     * default for the color scheme is' */
    static QColor defaultTextColor( QPainter * painter );

    void setTextBackgroundColor(const QColor &);
    QColor textBackgroundColor()const {return m_textBackColor;}

    /**
     * @return the point size to use on screen, given @p zh
     * This method takes care of superscript and subscript (smaller font).
     */
    float screenPointSize( const KoZoomHandler* zh ) const;

    /**
     * Returns the font metrics for the font used at the zoom & resolution
     * given by 'zh'. Despite the name, this is probably valid for printing too.
     * This method takes care of superscript and subscript (smaller font).
     */
    QFontMetrics screenFontMetrics( const KoZoomHandler* zh );

    /**
     * Returns the font to be used at the zoom & resolution given by 'zh'.
     * Despite the name, this is probably valid for printing too.
     * This method takes care of superscript and subscript (smaller font).
     */
    QFont screenFont( const KoZoomHandler* zh );

protected:
    virtual void generateKey();
    QColor m_textBackColor;
    class KoTextFormatPrivate;
    KoTextFormatPrivate *d;
};

/**
 * Our reimplementation of QTextFormatCollection, to provide KoTextFormats.
 */
class KoTextFormatCollection : public QTextFormatCollection
{
public:
    KoTextFormatCollection( const QFont & defaultFont );
    virtual ~KoTextFormatCollection();

    virtual QTextFormat *format( const QFont &f, const QColor &c );
    virtual void remove( QTextFormat *f );

    bool hasFormat( QTextFormat *f );

    virtual QTextFormat *createFormat( const QTextFormat &fm ) { return new KoTextFormat( static_cast<const KoTextFormat &>(fm) ); }
    virtual QTextFormat *createFormat( const QFont &f, const QColor &c ) { return new KoTextFormat( f, c, this ); }

private:
    class KoTextFormatCollectionPrivate;
    KoTextFormatCollectionPrivate *d;
};

#endif
