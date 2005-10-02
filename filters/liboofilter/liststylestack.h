/* This file is part of the KDE project
   Copyright (c) 2004 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef LISTSTYLESTACK_H
#define LISTSTYLESTACK_H

#include <qdom.h>
#include <qvaluestack.h>

/**
 * This class implements the list styles currently active at a given point.
 * Unlike StyleStack, this is NOT an inheritance stack.
 * The list-style stack contains one item per list level at any given point.
 * For instance inside \verbatim <ul><li><ul><li> \endverbatim (in html terms), it will have 2 items.
 */
class ListStyleStack
{
public:
    ListStyleStack();
    ~ListStyleStack();

    /**
     * Removes the style on top of the stack.
     */
    void pop();

    /**
     * Pushes the new list-style onto the stack.
     */
    void push( const QDomElement& style );

    /// @return true if we're inside a list (i.e. the stack isn't empty)
    bool hasListStyle() const { return !m_stack.isEmpty(); }

    /// @return currently applicable list style, i.e. the one on top of the stack
    /// Most list-level properties are the attributes of that element.
    QDomElement currentListStyle() const;

    /**
     * @return the style:properties for the currently applicable list style.
     * The list-level properties that are only "style" information,
     * like text:min-label-width, text:space-before, and style:font-name
     * are the attributes of that element.
     */
    QDomElement currentListStyleProperties() const;

    /**
     * Set the initial level of the list, i.e. of item at the bottom of the stack.
     * This is used when a level is explicitly specified in the
     * [un]ordered-list tag (OASIS extension)
     */
    void setInitialLevel( int initialLevel );

    /// @return initial level
    int initialLevel() const { return m_initialLevel; }

    /// @return current list level
    int level() const { return m_initialLevel + m_stack.count(); }


private:
    QValueStack<QDomElement> m_stack;
    int m_initialLevel;

};

#endif /* LISTSTYLESTACK_H */

