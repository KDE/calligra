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

#ifndef __ELEMENTINDEX_H
#define __ELEMENTINDEX_H

#include <iostream>
#include <memory>

#include "basicelement.h"

class ElementIndex;

typedef auto_ptr<ElementIndex> ElementIndexPtr;


/**
 * A type that describes an index. You can get one of those
 * for each index from an element that owns indexes.
 *
 * This type is used to work on indexes in a generic way.
 */
class ElementIndex {
public:

    virtual ~ElementIndex() { /*cerr << "ElementIndex destroyed.\n";*/ }
    
    /**
     * Moves the cursor inside the index. The index has to exist.
     */
    virtual void moveToIndex(FormulaCursor*, BasicElement::Direction) = 0;

    /**
     * Sets the cursor to point to the place where the index normaly
     * is. These functions are only used if there is no such index and
     * we want to insert them.
     */
    virtual void setToIndex(FormulaCursor*) = 0;

    /**
     * Tells whether we own those indexes.
     */
    virtual bool hasIndex() const = 0;

    /**
     * Tells to which element the index belongs.
     */
    virtual BasicElement* getElement() = 0;
};


#endif // __ELEMENTINDEX_H
