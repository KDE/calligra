/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

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

#ifndef KOOASISCONTEXT_H
#define KOOASISCONTEXT_H

class KoOasisStyles;
class QDomElement;
#include <koStyleStack.h>
#include "koliststylestack.h"

/**
 * Used during loading of Oasis format (and discarded at the end of the loading).
 * In addition to keeping a reference to the KoOasisStyles instance, this class
 * also has 'state' information - a stack with the currently used styles
 * (with its ancestors in the stack), another one with the list styles
 * currently applicable (one item in the stack per list level).
 *
 * @author David Faure <faure@kde.org>
 */
class KoOasisContext
{
public:
    /// Stores reference to the KoOasisStyles parsed by KoDocument.
    /// Make sure that the KoOasisStyles instance outlives this KoOasisContext instance.
    /// (This is the case during loaiding, when using the KoOasisStyles given by KoDocument)
    KoOasisContext( KoOasisStyles& styles );

    KoOasisStyles& oasisStyles() { return m_styles; }
    KoStyleStack& styleStack() { return m_styleStack; }

    void fillStyleStack( const QDomElement& object, const QString& attrName );
    void addStyles( const QDomElement* style );

    ///// List handling

    KoListStyleStack& listStyleStack() { return m_listStyleStack; }
    QString currentListStyleName() const { return m_currentListStyleName; }
    void setCurrentListStyleName( const QString& s ) { m_currentListStyleName = s; }

    /// Used for lists (numbered paragraphs)
    /// @return true on success (a list style was found and pushed)
    bool pushListLevelStyle( const QString& listStyleName, int level );
    /// Used for outline levels
    bool pushOutlineListLevelStyle( int level );

private:
    /// @return true on success (a list style was found and pushed)
    bool pushListLevelStyle( const QString& listStyleName, const QDomElement& fullListStyle, int level );

private:
    KoOasisStyles& m_styles;
    KoStyleStack m_styleStack;

    KoListStyleStack m_listStyleStack;
    QString m_currentListStyleName;
};

#endif /* KOOASISCONTEXT_H */

