/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __COMPLEXELEMENT_H
#define __COMPLEXELEMENT_H

#include "basicelement.h"


/**
 * An abstract base of all elements that have one or
 * indexes (or more but only two indexes have special
 * support.)
 */
class ComplexElement : public BasicElement {
public:

    ComplexElement(BasicElement* parent = 0);

    /**
     * Returns this if we are a complex element that knows how to
     * handle indexes.
     */
    virtual ComplexElement* getComplexElement() { return this; }

    // Moves the cursor inside the index. The index has to exist.
    virtual void moveToUpperIndex(FormulaCursor*, Direction) = 0;
    virtual void moveToLowerIndex(FormulaCursor*, Direction) = 0;

    // Sets the cursor to point to the place where the index normaly
    // is. These functions are only used if there is no such index and
    // we want to insert them.
    virtual void setToUpperIndex(FormulaCursor*) = 0;
    virtual void setToLowerIndex(FormulaCursor*) = 0;

    // Tells whether we own those indexes
    virtual bool hasUpperIndex() const = 0;
    virtual bool hasLowerIndex() const = 0;

    // Tells whether we are allowed to own them.
    // This is required as some elements just have one of them.
    virtual bool mightHaveUpperIndex() const { return true; }
    virtual bool mightHaveLowerIndex() const { return true; }
};

#endif // __COMPLEXELEMENT_H
