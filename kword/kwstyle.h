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

#ifndef kwstyle_h
#define kwstyle_h

#include "kwtextparag.h"

/**
 * A style is a combination of formatting attributes (font, color, etc.)
 * and paragraph-layout attributes, all grouped under a name.
 */
class KWStyle
{
public:
    // Create a blank style (with default attributes)
    KWStyle( const QString & name );

    // Create a style from a saved document
    KWStyle( QDomElement & styleElem );

    // Create a style from another style (UI for that is missing,
    // but this already happens when loading styles)
    //KWStyle( const KWStyle & style );

    QString name() const { return m_paragLayout.styleName(); }

    const KWParagLayout & paragLayout() const { return m_paragLayout; }
    KWParagLayout & paragLayout()  { return m_paragLayout; }

    // Return a format. Don't forget to use the format collection
    // of your QTextDocument from the result of that method.
    const QTextFormat & format() const { return m_format; }
    QTextFormat & format() { return m_format; }

    QString followingStyle() const { return m_followingStyle; }
    void setFollowingStyle( const QString & fst ) { m_followingStyle = fst; }

    void save( QDomElement parentElem );

private:
    KWParagLayout m_paragLayout;
    QTextFormat m_format;
    QString m_followingStyle;
};

#endif
