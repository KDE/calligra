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

#include "DiskDocumentHandler.hxx"

#include <string.h>

#ifdef USE_GSF_OUTPUT
#define PUTSTRING(M) gsf_output_puts(mpOutput, M)
DiskOdfDocumentHandler::DiskOdfDocumentHandler(GsfOutput *pOutput) :
#else
#define PUTSTRING(M) mpOutput->writeString(M)
DiskOdfDocumentHandler::DiskOdfDocumentHandler(FemtoZip *pOutput) :
#endif
	mpOutput(pOutput),
	mbIsTagOpened(false),
	msOpenedTagName()
{
}

void DiskOdfDocumentHandler::startElement(const char *psName, const WPXPropertyList &xPropList)
{
	if (mbIsTagOpened)
	{
		PUTSTRING(">");
		mbIsTagOpened = false;
	}
	PUTSTRING("<");
	PUTSTRING(psName);
	WPXPropertyList::Iter i(xPropList);
	for (i.rewind(); i.next(); )
	{
		// filter out libwpd elements
		if (strncmp(i.key(), "libwpd", 6) != 0)
		{
			PUTSTRING(" ");
			PUTSTRING(i.key());
			PUTSTRING("=\"");
			PUTSTRING(i()->getStr().cstr());
			PUTSTRING("\"");
		}

	}
	mbIsTagOpened = true;
	msOpenedTagName.sprintf("%s", psName);
}

void DiskOdfDocumentHandler::endElement(const char *psName)
{
	if (mbIsTagOpened)
	{
		if( msOpenedTagName == psName )
		{
			PUTSTRING("/>");
			mbIsTagOpened = false;
		}
		else // should not happen, but handle it
		{
			PUTSTRING(">");
			PUTSTRING("</");
			PUTSTRING(psName);
			PUTSTRING(">");
			mbIsTagOpened = false;
		}
	}
	else
	{
		PUTSTRING("</");
		PUTSTRING(psName);
		PUTSTRING(">");
		mbIsTagOpened = false;
	}
}

void DiskOdfDocumentHandler::characters(const WPXString &sCharacters)
{
	if (mbIsTagOpened)
	{
		PUTSTRING(">");
		mbIsTagOpened = false;
	}
	WPXString sEscapedCharacters(sCharacters, true);
	if (sEscapedCharacters.len() > 0)
		PUTSTRING(sEscapedCharacters.cstr());
}

void DiskOdfDocumentHandler::endDocument()
{
	if (mbIsTagOpened)
	{
		PUTSTRING(">");
		mbIsTagOpened = false;
	}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
