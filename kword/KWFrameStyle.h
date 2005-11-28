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

#include "koborder.h"

#include <qdom.h>
#include <qvaluelist.h>
#include <qbrush.h>
#include <qstringlist.h>
class KoOasisContext;
class KoSavingContext;
class KoGenStyles;
class KWFrameStyle;
class KWFrame;

/**
 * Base class for any kind of style that the user can create/modify/delete/display.
 * Use in conjunction with KoUserStyleCollection.
 */
class KoUserStyle
{
public:
    KoUserStyle( const QString & name );

    /// The internal name (used for loading/saving, but not shown to the user)
    /// Should be unique in a given style collection.
    QString name() const { return m_name; }
    /// Set the internal name - see generateUniqueName() if needed
    /// Should be unique in a given style collection.
    void setName( const QString & name ) { m_name = name; }

    /// The user-visible name (e.g. translated, or set by the user)
    QString displayName() const;
    /// Set the user-visible name
    void setDisplayName( const QString& name );

protected:
    QString m_name;
    QString m_displayName;
};

/**
 * Generic style collection class; to be used by other style collections and moved out.
 */
class KoUserStyleCollection
{
public:
    /**
     * @param prefix used by generateUniqueName to prefix new style names
     * (to avoid clashes between different kinds of styles)
     */
    KoUserStyleCollection( const QString& prefix );
    ~KoUserStyleCollection();

    /**
     * Erase all styles
     */
    void clear();

    /// @return true if the collection is empty
    bool isEmpty() const { return m_styleList.isEmpty(); }
    /// @return the number of items in the collection
    int count() const { return m_styleList.count(); }
    /// @return the index of @p style in the collection
    int indexOf( KoUserStyle* style ) const { return m_styleList.findIndex( style ); }

    const QValueList<KoUserStyle *> & styleList() const { return m_styleList; }

    QString generateUniqueName() const;

    /// Return the list composed of the display-name of each style in the collection
    QStringList displayNameList() const;

    /**
     * Find style based on the internal name @p name.
     * If the style with that name can't be found, then<br>
     * 1) if @p name equals @p defaultStyleName, return the first one, never 0<br>
     * 2) otherwise return 0
     */
    KoUserStyle* findStyle( const QString & name, const QString& defaultStyleName ) const;

    /**
     * Remove @p style from the collection. If the style isn't in the collection, nothing happens.
     * The style mustn't be deleted yet; it is stored into a list of styles to delete in clear().
     */
    void removeStyle( KoUserStyle *style );

    /**
     * Reorder the styles in the collection
     * @param list the list of internal names of the styles
     * WARNING, if an existing style isn't listed, it will be lost
     */
    void updateStyleListOrder( const QStringList& list );

    /**
     * Try adding @p sty to the collection.
     *
     * Either this succeeds, and @p sty is returned, or a style with the exact same
     * internal name and display name is present already, in which case the existing style
     * is updated, @p sty is deleted, and the existing style is returned.
     *
     * WARNING: @p sty can be deleted; use the returned value for any further processing.
     */
    KoUserStyle* addStyle( KoUserStyle* sty );

protected:
    KoUserStyleCollection( const KoUserStyleCollection& rhs ); // forbidden
    void operator=( const KoUserStyleCollection& rhs ); // forbidden

    QValueList<KoUserStyle *> m_styleList;
    QValueList<KoUserStyle *> m_deletedStyles;
    const QString m_prefix;
    mutable KoUserStyle *m_lastStyle; ///< Last style that was searched
};

/******************************************************************/
/* Class: KWFrameStyle                                            */
/******************************************************************/

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
 *
 */
class KWFrameStyleCollection : public KoUserStyleCollection
{
public:
    KWFrameStyleCollection();

    /// WARNING: slow method, since it has to convert the item types, avoid calling this.
    /// It should be only called for the old xml saving and for iteration by KWFrameStyleManager.
    QValueList<KWFrameStyle *> frameStyleList() const;

    /**
     * Find style based on the untranslated name @p name.
     * Overloaded for convenience
     */
    KWFrameStyle* findStyle( const QString & name ) const {
        return static_cast<KWFrameStyle*>( KoUserStyleCollection::findStyle( name, QString::fromLatin1( "Plain" ) ) );
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
    KWFrameStyle* frameStyleAt( int i ) { return static_cast<KWFrameStyle*>( m_styleList[i] ); }

    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;
    void loadOasisStyles( KoOasisContext& context );
};

#endif
