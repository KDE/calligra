/* This file is part of the KDE project
   Copyright (C) 2001 Shaheed Haque <srhaque@iee.org>

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

#include <qstring.h>
class KoTextParag;
class QDomElement;
namespace Qt3 { class QTextFormat; }
using namespace Qt3;

#ifndef koparagcounter_h
#define koparagcounter_h

/**
 * This is the structure associated with a paragraph (KoTextParag),
 * to define the bullet or numbering of the paragraph.
 */
class KoParagCounter
{
public:
    KoParagCounter();

    /** Invalidate the internal cache. Use it whenever the number associated with this
     * counter may have changed. */
    void invalidate();

    /** Return the current value of the counter either as a number or text. */
    int number( const KoTextParag *paragraph );
    QString text( const KoTextParag *paragraph );

    /** Work out the width of the text required for this counter. */
    int width( const KoTextParag *paragraph );

    /** XML support. */
    void load( QDomElement & element );
    void save( QDomElement & element );

    bool operator==( const KoParagCounter & c2 ) const;

    enum Numbering
    {
        NUM_NONE = 2,       // Unnumbered. Equivalent to there being
                            // no counter structure associated with a
                            // paragraph.
        NUM_LIST = 0,       // Numbered as a list item.
        NUM_CHAPTER = 1     // Numbered as a heading.
    };
    enum Style
    {
        STYLE_NONE = 0,
        STYLE_NUM = 1, STYLE_ALPHAB_L = 2, STYLE_ALPHAB_U = 3,
        STYLE_ROM_NUM_L = 4, STYLE_ROM_NUM_U = 5, STYLE_CUSTOMBULLET = 6,
        STYLE_CUSTOM = 7, STYLE_CIRCLEBULLET = 8, STYLE_SQUAREBULLET = 9,
        STYLE_DISCBULLET = 10
    };

    /** Numbering type and style. */
    Numbering numbering() const;
    void setNumbering( Numbering n );

    Style style() const;
    void setStyle( Style s );

    /** Does this counter have a bullet style? */
    bool isBullet() const;

    /** The level of the numbering.
     * Depth of 0 means the major numbering. (1, 2, 3...)
     * Depth of 1 is 1.1, 1.2, 1.3 etc. */
    unsigned int depth() const;
    void setDepth( unsigned int d );

    /** Starting number. */
    int startNumber() const;
    void setStartNumber( int s );

    /** Prefix and suffix strings. */
    QString prefix() const;
    void setPrefix( QString p );
    QString suffix() const;
    void setSuffix( QString s );

    /** The character and font for STYLE_CUSTOMBULLET. */
    QChar customBulletCharacter() const;
    void setCustomBulletCharacter( QChar c );
    QString customBulletFont() const;
    void setCustomBulletFont( QString f );

    /** The string STYLE_CUSTOM. */
    QString custom() const;
    void setCustom( QString c );


    static QString makeRomanNumber( int n );
    static QString makeAlphaUpperNumber( int n );
    static QString makeAlphaLowerNumber( int n );

private:

    /** Return our parent paragraph, if there is such a thing. For a paragraph "1.1.",
     * the parent is the paragraph numbered "1.". */
    KoTextParag *parent( const KoTextParag *paragraph );

    Numbering m_numbering;
    Style m_style;
    unsigned int m_depth;
    int m_startNumber;
    QString m_prefix;
    QString m_suffix;
    struct
    {
        QChar character;
        QString font;
    } m_customBullet;
    QString m_custom;

    /** The cached, calculated values for this counter:
     *
     *  VALUE                                       VALUE WHEN INVALID
     *  number of this counter                           -1
     *  text of this counter                             QString::null
     *  width of the label                               -1
     *  parent                                           (KoTextParag *)-1
     *  the format that was used to calculate the width  0
     */
    struct
    {
        int number;
        QString text;
        int width;
        KoTextParag *parent;
        QTextFormat * counterFormat;
    } m_cache;
};

#endif
