/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef __OPERATORELEMENT_H
#define __OPERATORELEMENT_H

#include "textelement.h"


/**
 * An textelement that has spaces before and after the char.
 */
class OperatorElement : public TextElement {
public:
    OperatorElement(QChar ch);

    /**
     * Calculates our width and height and
     * our children's parentPosition.
     */
    virtual void calcSizes(ContextStyle& context, int parentSize);

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, ContextStyle& context, int parentSize, const QPoint& parentOrigin);

    virtual QDomElement getElementDom(QDomDocument *doc);



private:

    /**
     * The width that is added to the TextElement's width.
     */
    int spaceWidth;
};


#endif // __OPERATORELEMENT_H
