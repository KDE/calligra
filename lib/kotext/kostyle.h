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

#include "qrichtext_p.h" // for KoTextFormat, KoParagLayout
#include <qdom.h>
#include <qptrlist.h>

class KoStyle;

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
/**
 * A style is a combination of formatting attributes (font, color, etc.)
 * and paragraph-layout attributes, all grouped under a name.
 */
class KoStyle
{
public:
    /** Create a blank style (with default attributes) */
    KoStyle( const QString & name );

    /** Copy another style */
    KoStyle( const KoStyle & rhs ) { *this = rhs; }

    ~KoStyle() {}

    void operator=( const KoStyle & );

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

    const KoParagLayout & paragLayout() const { return m_paragLayout; }
    KoParagLayout & paragLayout()  { return m_paragLayout; }

    /** Return a format. Don't forget to use the format collection
     * of your KoTextDocument from the result of that method. */
    const KoTextFormat & format() const { return m_format; }
    KoTextFormat & format() { return m_format; }

    KoStyle *followingStyle() { return m_followingStyle; }
    void setFollowingStyle( KoStyle *fst ) { m_followingStyle = fst; }

    void save( QDomElement parentElem, KoZoomHandler* zh );
    void saveStyle( QDomElement & parentElem );
    static KoParagLayout loadStyle( QDomElement & parentElem, int docVersion=2 );

    static int getAttribute(QDomElement &element, const char *attributeName, int defaultValue)
      {
	QString value;
	if ( ( value = element.attribute( attributeName ) ) != QString::null )
	  return value.toInt();
	else
	  return defaultValue;
      }

    static double getAttribute(QDomElement &element, const char *attributeName, double defaultValue)
      {
	QString value;
	if ( ( value = element.attribute( attributeName ) ) != QString::null )
	  return value.toDouble();
	else
	  return defaultValue;
      }

private:
    KoParagLayout m_paragLayout;
    QString m_name;
    QString m_shortCut_name;
    KoTextFormat m_format;
    KoStyle *m_followingStyle;
};

#endif
