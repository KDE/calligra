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

#include <qfontmetrics.h>
#include <qpoint.h>

#include "operatorelement.h"


OperatorElement::OperatorElement(QChar ch, BasicElement* parent)
    : TextElement(ch, parent)
{
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void OperatorElement::calcSizes(ContextStyle& context, int parentSize)
{
    TextElement::calcSizes(context, parentSize);
    QFontMetrics fm(context.getDefaultFont());
    spaceWidth = fm.width('0');
    setWidth(getWidth()+spaceWidth);
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void OperatorElement::draw(QPainter& painter, ContextStyle& context, int parentSize, const QPoint& parentOrigin)
{
    TextElement::draw(painter, context, parentSize,
                      QPoint(parentOrigin.x()+spaceWidth/2, parentOrigin.y()));
}

QFont OperatorElement::getFont(ContextStyle& context)
{
    return context.getDefaultSymbolFont();
}

QDomElement OperatorElement::getElementDom(QDomDocument *doc)
{
    QDomElement de=doc->createElement("OPERATOR");
    de.appendChild(TextElement::getElementDom(doc));
    return de;
}

bool OperatorElement::buildFromDom(QDomElement *elem)
{
    // checking
    if (elem->tagName() != "OPERATOR") {
        cerr << "Wrong tag name " << elem->tagName().latin1() << "for OperatorElement.\n";
        return false;
    }

    // get attributes

    // read parent
    QDomNode n = elem->firstChild();
    if (n.isElement()) {
        QDomElement e = n.toElement();
        if (!TextElement::buildFromDom(&e)) {
            return false;
        }
    }
    else {
        return false;
    }
    n = n.nextSibling();
    return true;
}
