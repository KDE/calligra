/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* writerperfect
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2002-2004 William Lachance (wrlach@gmail.com)
 * Copyright (C) 2004-2006 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#include "StdOutHandler.hxx"

#include <string.h>
#include <stdio.h>

StdOutHandler::StdOutHandler() :
	mbIsTagOpened(false), msOpenedTagName()
{
	printf("<?xml version=\"1.0\"?>\n");
}

void StdOutHandler::startElement(const char *psName, const librevenge::RVNGPropertyList &xPropList)
{
	if (mbIsTagOpened)
	{
		printf(">");
		mbIsTagOpened = false;
	}
	printf("<%s", psName);
	librevenge::RVNGPropertyList::Iter i(xPropList);
	for (i.rewind(); i.next(); )
	{
		// filter out librevenge properties
		if (strncmp(i.key(), "librevenge", 10) != 0)
			printf(" %s=\"%s\"", i.key(), i()->getStr().cstr());
	}
	mbIsTagOpened = true;
	msOpenedTagName.sprintf("%s", psName);
}

void StdOutHandler::endElement(const char *psName)
{
	if (mbIsTagOpened)
	{
		if( msOpenedTagName == psName )
		{
			printf("/>");
			mbIsTagOpened = false;
		}
		else // should not happen, but handle it
		{
			printf(">");
			printf("</%s>", psName);
			mbIsTagOpened = false;
		}
	}
	else
	{
		printf("</%s>", psName);
		mbIsTagOpened = false;
	}
}

void StdOutHandler::characters(const librevenge::RVNGString &sCharacters)
{
	if (mbIsTagOpened)
	{
		printf(">");
		mbIsTagOpened = false;
	}
	printf("%s", librevenge::RVNGString::escapeXML(sCharacters).cstr());
}

void StdOutHandler::endDocument()
{
	if (mbIsTagOpened)
	{
		printf(">");
		mbIsTagOpened = false;
	}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
