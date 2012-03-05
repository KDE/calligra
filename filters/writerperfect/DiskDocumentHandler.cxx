/* writerperfect:
 *
 * Copyright (C) 2002-2004 William Lachance (wrlach@gmail.com)
 * Copyright (C) 2004-2006 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
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
	mbIsTagOpened(false)
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
