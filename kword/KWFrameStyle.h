/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>
                 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwframestyle_h
#define kwframestyle_h

#include <KoUserStyle.h>
#include <KoUserStyleCollection.h>

#include "KoBorder.h"
#include <qbrush.h>

class QDomElement;
class KoOasisContext;
class KoSavingContext;
class KoGenStyles;
class KWFrameStyle;
class KWFrame;

/**
 * The user-visible style for frames
 * (borders and background)
 */
class KWFrameStyle : public KoUserStyle
{
public:
    /** Create a blank framestyle (with default attributes) */
    KWFrameStyle( const QString & name );

    KWFrameStyle( const QString & name, KWFrame * frame );
    KWFrameStyle( QDomElement & parentElem, int docVersion=2 );

    /** Copy another framestyle */
    KWFrameStyle( const KWFrameStyle & rhs );
    void operator=( const KWFrameStyle& rhs );

    ~KWFrameStyle() {}

    enum { Borders = 1,
           Background = 2
    } Flags;

    int compare( const KWFrameStyle & frameStyle ) const;

    // ATTRIBUTES
    QBrush backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor( const QBrush &_color ) { m_backgroundColor = _color; }

    const KoBorder & leftBorder() const { return m_borderLeft; }
    void setLeftBorder( KoBorder _left )  { m_borderLeft = _left; }

    const KoBorder & rightBorder() const { return m_borderRight; }
    void setRightBorder( KoBorder _right )  { m_borderRight = _right; }

    const KoBorder & topBorder() const { return m_borderTop; }
    void setTopBorder( KoBorder _top )  { m_borderTop = _top; }

    const KoBorder & bottomBorder() const { return m_borderBottom; }
    void setBottomBorder( KoBorder _bottom )  { m_borderBottom = _bottom; }

    /// save (old xml format)
    void saveFrameStyle( QDomElement & parentElem );
    /// save (new oasis xml format)
    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;

    /// load (old xml format)
    static KWFrameStyle *loadStyle( QDomElement & parentElem, int docVersion=2 );
    /// load (new oasis xml format)
    void loadOasis( QDomElement & styleElem, KoOasisContext& context );

private:
    QBrush m_backgroundColor;
    KoBorder m_borderLeft, m_borderRight, m_borderTop, m_borderBottom;
};

/**
 * Collection of frame styles
 */
class KWFrameStyleCollection : public KoUserStyleCollection
{
public:
    KWFrameStyleCollection();

    static QString defaultStyleName() { return QString::fromLatin1( "Plain" ); }

    /**
     * Find style based on the untranslated name @p name.
     * Overloaded for convenience
     */
    KWFrameStyle* findStyle( const QString & name ) const {
        return static_cast<KWFrameStyle*>( KoUserStyleCollection::findStyle( name, defaultStyleName() ) );
    }

    /**
     * Find style based on the display name @p name.
     * This is only for the old XML loading.
     * Overloaded for convenience
     */
    KWFrameStyle* findStyleByDisplayName( const QString & name ) const {
        return static_cast<KWFrameStyle*>( KoUserStyleCollection::findStyleByDisplayName( name ) );
    }

    /**
     * See KoUserStyleCollection::addStyle.
     * Overloaded for convenience.
     */
    KWFrameStyle* addStyle( KWFrameStyle* sty ) {
        return static_cast<KWFrameStyle*>( KoUserStyleCollection::addStyle( sty ) );
    }

    /**
     * Return style number @p i.
     */
    KWFrameStyle* frameStyleAt( int i ) const {
        return static_cast<KWFrameStyle*>( m_styleList[i] );
    }

    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;
    int loadOasisStyles( KoOasisContext& context );
};

#endif
