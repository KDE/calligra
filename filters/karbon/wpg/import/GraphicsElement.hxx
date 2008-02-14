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
 * For further information visit http://libwpd.sourceforge.net
 *
 */

/* "This product is not manufactured, approved, or supported by 
 * Corel Corporation or Corel Corporation Limited."
 */

#ifndef _GRAPHICSELEMENT_H
#define _GRAPHICSELEMENT_H
#include <string>
#include <map>
#include <vector>

#include "GraphicsHandler.hxx"

class GraphicsElement
{
public:	
	virtual ~GraphicsElement() {}
	virtual void write(GraphicsHandler *pHandler) const = 0;
	virtual void print() const {}
};

class TagGraphicsElement : public GraphicsElement
{
public:
	explicit TagGraphicsElement(const char *szTagName) : msTagName(szTagName) {}
	const std::string &getTagName() const { return msTagName; }
	virtual void print() const;
private:
	const std::string msTagName;
};

class OpenTagGraphicsElement : public TagGraphicsElement
{
public:
	explicit OpenTagGraphicsElement(const char *szTagName) : TagGraphicsElement(szTagName) {}
	~OpenTagGraphicsElement() {}
	void addAttribute(const std::string &szAttributeName, const std::string &sAttributeValue);
	virtual void write(GraphicsHandler *pHandler) const;
	virtual void print () const;
private:
	std::vector<std::pair<std::string, std::string> > maAttrList;
};

class CloseTagGraphicsElement : public TagGraphicsElement
{
public:
	explicit CloseTagGraphicsElement(const char *szTagName) : TagGraphicsElement(szTagName) {}
	virtual void write(GraphicsHandler *pHandler) const;
};

class CharDataGraphicsElement : public GraphicsElement
{
public:
	CharDataGraphicsElement(const char *sData) : GraphicsElement(), msData(sData) {}
	virtual void write(GraphicsHandler *pHandler) const;
private:
	std::string msData;
};


#endif
