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

#include "contextstyle.h"
#include "textsymbolelement.h"


TextSymbolElement::TextSymbolElement(QChar ch, BasicElement* parent)
        : TextElement(ch, parent)
{
}


/**
 * @returns the font to be used for the element.
 */
QFont TextSymbolElement::getFont(ContextStyle& context)
{
    return context.getSymbolFont();
}

/**
 * @returns the space to be left before and behind the character.
 */
int TextSymbolElement::getSpaceWidth(ContextStyle&, int)
{
    return 0;
}

/**
 * Sets up the painter to be used for drawing.
 */
void TextSymbolElement::setUpPainter(ContextStyle&, QPainter&)
{
}


/**
 * Appends our attributes to the dom element.
 */
void TextSymbolElement::writeDom(QDomElement& element)
{
    TextElement::writeDom(element);
    //element.setAttribute("NAME", name);
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool TextSymbolElement::readAttributesFromDom(QDomElement& element)
{
    if (!TextElement::readAttributesFromDom(element)) {
        return false;
    }
    
    //name = element.attribute("NAME");
    //return !name.isNull();
    return true;
}
