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

#ifndef kostyle_h
#define kostyle_h

#include "kotextformat.h"
#include "koparaglayout.h"
#include <qdom.h>
#include <qptrlist.h>

class KoStyle;
class KoOasisContext;

struct StyleChangeDef {
    StyleChangeDef() {
        paragLayoutChanged = -1;
        formatChanged = -1;
    }
    StyleChangeDef( int parag, int format) {
        paragLayoutChanged = parag;
        formatChanged = format;
    };
    int paragLayoutChanged;
    int formatChanged;
};
typedef QMap<KoStyle *, StyleChangeDef> StyleChangeDefMap;

class KoStyleCollection
{
public:
    KoStyleCollection();
    ~KoStyleCollection();
    const QPtrList<KoStyle> & styleList() const { return m_styleList; }

    KoStyle* findStyle( const QString & name );
    KoStyle* findStyleShortCut( const QString & _shortCut );
    /**
     * Return style number @p i.
     */
    KoStyle* styleAt( int i ) { return m_styleList.at(i); }

    KoStyle* addStyleTemplate( KoStyle *style );

    void removeStyleTemplate ( KoStyle *style );

    void updateStyleListOrder( const QStringList &list );

private:
    QPtrList<KoStyle> m_styleList;
    QPtrList<KoStyle> m_deletedStyles;
    static int styleNumber;
    KoStyle *m_lastStyle;
};

class KoCharStyle
{
public:
/** Create a blank style (with default attributes) */
    KoCharStyle( const QString & name );

    /** Copy another style */
    KoCharStyle( const KoCharStyle & rhs ) { *this = rhs; }

    virtual ~KoCharStyle() {}

    /** Return a format. Don't forget to use the format collection
     * of your KoTextDocument from the result of that method. */
    const KoTextFormat & format() const;
    KoTextFormat & format();

    void operator=( const KoCharStyle & );

    /** The internal name (untranslated if a standard style) */
    QString name() const { return m_name; }
    void setName( const QString & name ) { m_name = name; }
    /** The translated name */
    QString translatedName() const;

    QString shortCutName() const {
        return m_shortCut_name;
    }

    void setShortCutName( const QString & _shortCut) {
        m_shortCut_name=_shortCut;
    }

protected:
    QString m_name;
    QString m_shortCut_name;
    KoTextFormat m_format;
};

/**
 * A style is a combination of formatting attributes (font, color, etc.)
 * and paragraph-layout attributes, all grouped under a name.
 */
class KoStyle : public KoCharStyle
{
public:
    /** Create a blank style (with default attributes) */
    KoStyle( const QString & name );

    /** Copy another style */
    KoStyle( const KoStyle & rhs );

    ~KoStyle() {}

    void operator=( const KoStyle & );


    const KoParagLayout & paragLayout() const;
    KoParagLayout & paragLayout();

    KoStyle *followingStyle() const { return m_followingStyle; }
    void setFollowingStyle( KoStyle *fst ) { m_followingStyle = fst; }

    /// Saves the name, layout, the following style and the outline bool. Not the format.
    /// @deprecated
    void saveStyle( QDomElement & parentElem );
    /// Loads the name, layout and the outline bool. Not the "following style" nor the format.
    /// @deprecated
    void loadStyle( QDomElement & parentElem, int docVersion = 2 );

    /// Load the style from OASIS
    void loadStyle( QDomElement & styleElem, KoOasisContext& context );

    static int getAttribute(const QDomElement &element, const char *attributeName, int defaultValue)
      {
	QString value = element.attribute( attributeName );
	return value.isNull() ? defaultValue : value.toInt();
      }

    static double getAttribute(const QDomElement &element, const char *attributeName, double defaultValue)
      {
	QString value = element.attribute( attributeName );
	return value.isNull() ? defaultValue : value.toDouble();
      }

    KoStyle * parentStyle() const {return m_parentStyle;}
    void setParentStyle( KoStyle *_style){ m_parentStyle = _style;}

    int inheritedParagLayoutFlag() const { return m_inheritedParagLayoutFlag; }
    int inheritedFormatFlag() const { return m_inheritedFormatFlag; }

    void propagateChanges( int paragLayoutFlag, int formatFlag );

    // If true, paragraphs with this style will be included in the table of contents
    bool isOutline() const { return m_bOutline; }
    void setOutline( bool b );

private:
    KoParagLayout m_paragLayout;
    KoStyle *m_followingStyle;
    KoStyle *m_parentStyle;
    int m_inheritedParagLayoutFlag;
    int m_inheritedFormatFlag;
    bool m_bOutline;
};

#endif
