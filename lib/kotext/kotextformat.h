// -*- c++ -*-
// File included by qrichtext_p.h to add our own methods to KoTextFormat
// Method implementations are in kotextformat.cc

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

class KoTextFormatPrivate
{
public:
    KoTextFormatPrivate() : m_screenFont( 0L ), m_screenFontMetrics( 0L ),
                            m_screenFontNoZoom( 0L ), m_screenFontMetricsNoZoom( 0L )
    {}
    ~KoTextFormatPrivate()
    {
        clearCache();
    }
    void clearCache()
    {
        delete m_screenFontMetrics; m_screenFontMetrics = 0L;
        delete m_screenFont; m_screenFont = 0L;
        delete m_screenFontMetricsNoZoom; m_screenFontMetricsNoZoom = 0L;
        delete m_screenFontNoZoom; m_screenFontNoZoom = 0L;
    }
    // caching for speedup when formatting
    QFont* m_screenFont; // font to be used when painting (zoom-dependent)
    QFontMetrics* m_screenFontMetrics; // font metrics on screen (zoom-dependent)
    QFont* m_screenFontNoZoom; // font to be used when formatting text for layout units
    QFontMetrics* m_screenFontMetricsNoZoom; // font metrics for m_screenFontMetricsNoZoom
};


/**
 * Our extension of KoTextFormat, to add support for storing StrikeOut and
 * TextBackgroundColor as part of the format, and for caching of the appropriate
 * fontmetrics for WYSIWYG (text layout and painting).
 */
public:
    /** Return a set of flags showing the differences between this and 'format' */
    int compare( const KoTextFormat & format ) const;

    /** Extending the base KoTextFormat enum */
    enum { StrikeOut = 512, TextBackgroundColor = 1024, ExtendUnderLine = 2048 , SpellCheckingLanguage = 4096};

    enum UnderlineLineType { U_NONE = 0, U_SIMPLE = 1, U_DOUBLE = 2, U_SIMPLE_BOLD = 3};
    enum StrikeOutLineType { S_NONE = 0, S_SIMPLE = 1, S_DOUBLE = 2};
    enum UnderlineLineStyle { U_SOLID = 0 , U_DASH = 1, U_DOT = 2, U_DASH_DOT = 3, U_DASH_DOT_DOT = 4};
    enum StrikeOutLineStyle { S_SOLID = 0 , S_DASH = 1, S_DOT = 2, S_DASH_DOT = 3, S_DASH_DOT_DOT = 4};

    /** Set a decimal point size. NOTE: this value isn't stored in the formay key.
        You should NOT call this - it's a special treat for KoTextFormatter */
    void setPointSizeFloat( float );
    //float pointSizeFloat() const { return fn.pointSizeFloat(); }

    /** Call this when a text color is set to 'invalid', meaning 'whatever the
     * default for the color scheme is' */
    static QColor defaultTextColor( QPainter * painter );

    void setStrikeOutLineType (StrikeOutLineType _type);
    StrikeOutLineType strikeOutLineType()const {return m_strikeOutLine;}


    void setStrikeOutLineStyle( StrikeOutLineStyle _type );
    StrikeOutLineStyle strikeOutLineStyle()const {return m_strikeOutLineStyle;}


    void setTextBackgroundColor(const QColor &);
    QColor textBackgroundColor()const {return m_textBackColor;}

    void setTextUnderlineColor(const QColor &);
    QColor textUnderlineColor()const {return m_textUnderlineColor;}

    void setUnderlineLineType (UnderlineLineType _type);
    UnderlineLineType underlineLineType()const {return m_underlineLine;}

    void setUnderlineLineStyle (UnderlineLineStyle _type);
    UnderlineLineStyle underlineLineStyle()const {return m_underlineLineStyle;}

    void setSpellCheckingLanguage( const QString & _lang);
    QString spellCheckingLanguage() const { return m_spellCheckLanguage;}

    bool doubleUnderline() const { return (m_underlineLine==U_DOUBLE ); }
    bool underline() const { return (m_underlineLine==U_SIMPLE ); }
    bool strikeOut() const { return (m_strikeOutLine==S_SIMPLE ); }
    bool doubleStrikeOut() const { return (m_strikeOutLine==S_DOUBLE ); }
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
     * Used by KoTextFormatter twice: once for the 100% zoom pointsize (via charWidthLU),
     * and once for the current zoom pointsize.
     */
    int charWidth( const KoZoomHandler* zh, bool applyZoom, const KoTextStringChar* c,
                   const KoTextParag* parag, int i ) const;

    /**
     * Return the width of one char in LU pixels.
     * Equivalent to ptToLayoutUnitPt( charWidth( 0L, false, c, parag, i ) )
     */
    int charWidthLU( const KoTextStringChar* c,
                     const KoTextParag* parag, int i ) const;

    /**
     * Return the width of one char (from a string, not necessarily from a paragraph) in LU pixels.
     * Do not call this for custom items, or for complex glyphs.
     * But this can still be used for ' ' (for parag counters), 'x' (for tabs) etc.
     */
    //int width( const QChar &ch ) const; // already declared in qrichtext_p.h
    static QString underlineStyleToString( UnderlineLineStyle _lineType );
    static QString strikeOutStyleToString( StrikeOutLineStyle _lineType );
    static UnderlineLineStyle stringToUnderlineStyle( const QString & _str );
    static StrikeOutLineStyle stringToStrikeOutStyle( const QString & _str );

protected:
    QColor m_textBackColor;
    QColor m_textUnderlineColor;
    UnderlineLineType m_underlineLine;
    StrikeOutLineType m_strikeOutLine;
    UnderlineLineStyle m_underlineLineStyle;
    StrikeOutLineStyle m_strikeOutLineStyle;
    QString m_spellCheckLanguage;
    class KoTextFormatPrivate;
    KoTextFormatPrivate *d;
