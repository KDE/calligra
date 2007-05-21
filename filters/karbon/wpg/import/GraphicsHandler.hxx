/*
 * Copyright (C) 2004 William Lachance (wlach@interlog.com)
 * Copyright (C) 2004 Net Integration Technologies (http://www.net-itech.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA  02111-1301 USA
 *
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */
#ifndef _GRAPHICSHANDLER_H
#define _GRAPHICSHANDLER_H
#include <string>
#include <map>
#include <vector>

class GraphicsHandler
{
public:
	virtual ~GraphicsHandler() {}
        virtual void startDocument() = 0;
        virtual void endDocument() = 0;
        virtual void startElement(const char *psName, const std::vector< std::pair <std::string, std::string> > &xPropList) = 0;
        virtual void endElement(const char *psName) = 0;
        virtual void characters(const std::string &sCharacters) = 0;
};
#endif
