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
    KoTextFormat( const QFont &f, const QColor &c, Qt3::QTextFormatCollection * coll );
    KoTextFormat( const KoTextFormat &fm );
    KoTextFormat& operator=( const KoTextFormat &fm );

    virtual void copyFormat( const QTextFormat & nf, int flags );

    /** Return a set of flags showing the differences between this and 'format' */
    int compare( const KoTextFormat & format ) const;

    /** Extending QTextFormat's enum */
    enum { StrikeOut = 512, TextBackgroundColor = 1024 };

    /** Set a decimal point size. NOTE: this value isn't stored in the formay key.
        You should NOT call this - it's a special treat for KoTextFormatter */
    void setPointSizeFloat( float );
    //float pointSizeFloat() const { return fn.pointSizeFloat(); }

    /** Call this when a text color is set to 'invalid', meaning 'whatever the
     * default for the color scheme is' */
    static QColor defaultTextColor( QPainter * painter );

    void setStrikeOut(bool);

    void setTextBackgroundColor(const QColor &);
    QColor textBackgroundColor()const {return m_textBackColor;}

    /**
     * @return the point size to use on screen, given @p zh
     * This method takes care of superscript and subscript (smaller font).
     * @param applyZoom if false, retrieve fontsize for 100%-zoom (for kotextformatter)
     */
    float screenPointSize( const KoZoomHandler* zh, bool applyZoom ) const;

    /**
     * Returns the font metrics for the font used at the zoom & resolution
     * given by 'zh'. Despite the name, this is probably valid for printing too.
     * This method takes care of superscript and subscript (smaller font).
     * @param applyZoom if false, retrieve fontsize for 100%-zoom (for kotextformatter)
     */
    QFontMetrics screenFontMetrics( const KoZoomHandler* zh, bool applyZoom = true ) const;

    /**
     * Returns the font to be used at the zoom & resolution given by 'zh'.
     * Despite the name, this is probably valid for printing too.
     * This method takes care of superscript and subscript (smaller font).
     * @param applyZoom if false, retrieve fontsize for 100%-zoom (for kotextformatter)
     */
    QFont screenFont( const KoZoomHandler* zh, bool applyZoom = true ) const;

    /**
     * Return the width of one char in one paragraph.
     * Used by KoTextFormatter twice: once for the 100% zoom pointsize,
     * and once for the current zoom pointsize.
     */
    int charWidth( const KoZoomHandler* zh, bool applyZoom, const KoTextStringChar* c,
                   const Qt3::QTextParag* parag, int i ) const;

    virtual int height() const;
    virtual int ascent() const;
    virtual int descent() const;

protected:
    virtual void generateKey();
    QColor m_textBackColor;
    class KoTextFormatPrivate;
    KoTextFormatPrivate *d;
};

/**
 * Our reimplementation of QTextFormatCollection, to provide KoTextFormats.
 */
class KoTextFormatCollection : public Qt3::QTextFormatCollection
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
