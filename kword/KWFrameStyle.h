/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

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
#include <qptrlist.h>
#include <qbrush.h>
#include <qstringlist.h>
class KoOasisContext;
class KoSavingContext;
class KoGenStyles;
class KWFrameStyle;
class KWFrame;

class KWGenericStyle
{
public:
    KWGenericStyle( const QString & name );

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
class KWGenericStyleCollection
{
public:
    KWGenericStyleCollection();
    ~KWGenericStyleCollection();

    void clear();

    bool isEmpty() const { return m_styleList.isEmpty(); }
    int count() const { return m_styleList.count(); }
    int indexOf( KWGenericStyle* style ) /*const*/ { return m_styleList.findRef( style ); }

    const QPtrList<KWGenericStyle> & styleList() const { return m_styleList; }

    QString generateUniqueName() const;

    /// Return the list composed of the display-name of each style in the collection
    QStringList displayNameList() const;

    /**
     * Find style based on the internal name @p name
     * If the style with that name can't be found, then
     * 1) if @p name is @defaultStyleName, return the first one, never 0
     * 2) otherwise return 0
     */
    KWGenericStyle* findStyle( const QString & name, const QString& defaultStyleName ) const;

    void removeStyle( KWGenericStyle *style );
    void updateStyleListOrder( const QStringList& list );
    KWGenericStyle* addStyle( KWGenericStyle* sty );

    // TODO move many methods here

protected:
    // TODO switch to QValueList<KWGenericStyle *> in preparation for KDE4
    QPtrList<KWGenericStyle> m_styleList;
    QPtrList<KWGenericStyle> m_deletedStyles;
    mutable KWGenericStyle *m_lastStyle; ///< Last style that was searched
};

/******************************************************************/
/* Class: KWFrameStyle                                            */
/******************************************************************/

class KWFrameStyle : public KWGenericStyle
{
public:
    /** Create a blank framestyle (with default attributes) */
    KWFrameStyle( const QString & name );

    KWFrameStyle( const QString & name, KWFrame * frame );
    KWFrameStyle( QDomElement & parentElem, int docVersion=2 );

    /** Copy another framestyle */
    //KWFrameStyle( const KWFrameStyle & rhs ) { *this = rhs; }

    ~KWFrameStyle() {}

    enum { Borders = 1,
           Background = 2
    } Flags;

    void operator=( const KWFrameStyle & );
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

    // SAVING METHODS
    void saveFrameStyle( QDomElement & parentElem );
    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;

    // LOADING METHODS
    static KWFrameStyle *loadStyle( QDomElement & parentElem, int docVersion=2 );
    void loadOasis( QDomElement & styleElem, KoOasisContext& context );

private:
    QBrush m_backgroundColor;
    KoBorder m_borderLeft, m_borderRight, m_borderTop, m_borderBottom;
};

/******************************************************************/
/* Class: KWFrameStyleCollection                                  */
/******************************************************************/

class KWFrameStyleCollection : public KWGenericStyleCollection
{
public:
    KWFrameStyleCollection();

    /// WARNING: slow method, since it has to convert the item types, avoid calling this.
    /// It should be only called for the old xml saving and for iteration by KWFrameStyleManager.
    QValueList<KWFrameStyle *> frameStyleList() const;

    /**
     * Find style based on the untranslated name @p name
     */
    KWFrameStyle* findStyle( const QString & name ) const {
        return static_cast<KWFrameStyle*>( KWGenericStyleCollection::findStyle( name, QString::fromLatin1( "Plain" ) ) );
    }

    /**
     * Return style number @p i.
     */
    KWFrameStyle* frameStyleAt( int i ) { return static_cast<KWFrameStyle*>( m_styleList.at(i) ); }

    void saveOasis( KoGenStyles& mainStyles, KoSavingContext& savingContext ) const;
    void loadOasisStyles( KoOasisContext& context );
};

#endif
