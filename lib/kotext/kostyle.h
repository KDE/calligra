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

#include "kotextparag.h"
#include "kotextformat.h"

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
    void operator=( const KoStyle & );

    /** The internal name (untranslated if a standard style) */
    QString name() const { return m_name; }
    void setName( const QString & name ) { m_name = name; }
    /** The translated name */
    QString translatedName() const;

    const KoParagLayout & paragLayout() const { return m_paragLayout; }
    KoParagLayout & paragLayout()  { return m_paragLayout; }

    /** Return a format. Don't forget to use the format collection
     * of your QTextDocument from the result of that method. */
    const KoTextFormat & format() const { return m_format; }
    KoTextFormat & format() { return m_format; }

    KoStyle *followingStyle() { return m_followingStyle; }
    void setFollowingStyle( KoStyle *fst ) { m_followingStyle = fst; }

    void save( QDomElement parentElem, KoZoomHandler* zh );

private:
    KoParagLayout m_paragLayout;
    QString m_name;
    KoTextFormat m_format;
    KoStyle *m_followingStyle;
};

#endif
