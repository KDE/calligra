/* GraphicsElement: The items we are collecting to be put into the Writer
 * document: paragraph and spans of text, as well as section breaks.
 *
 * Copyright (C) 2002-2003 William Lachance (william.lachance@sympatico.ca)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA  02111-1301 USA
 *
 * For further information visit http://libwpg.sourceforge.net
 *
 */

/* "This product is not manufactured, approved, or supported by 
 * Corel Corporation or Corel Corporation Limited."
 */

#include "GraphicsElement.hxx"
#include "GraphicsHandler.hxx"
#include <string.h>

#define ASCII_SPACE 0x0020

void TagGraphicsElement::print() const
{
}

void OpenTagGraphicsElement::write(GraphicsHandler *pHandler) const
{
	pHandler->startElement(getTagName().c_str(), maAttrList);
}

void OpenTagGraphicsElement::print() const
{ 
	TagGraphicsElement::print(); 	
}

void OpenTagGraphicsElement::addAttribute(const std::string &szAttributeName, const std::string &sAttributeValue)
{
	std::pair<std::string, std::string> tmpAttribute;
	tmpAttribute.first = szAttributeName;
	tmpAttribute.second = sAttributeValue;
        maAttrList.push_back(tmpAttribute);
}

void CloseTagGraphicsElement::write(GraphicsHandler *pHandler) const
{

	pHandler->endElement(getTagName().c_str());
}

void CharDataGraphicsElement::write(GraphicsHandler *pHandler) const
{
	pHandler->characters(msData);
}
