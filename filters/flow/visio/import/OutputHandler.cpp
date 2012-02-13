/* This file is part of the KDE project
 * Copyright (C) 2011 Yue Liu <yue.liu@mail.com>
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

#include "OutputHandler.h"

#include <string.h>

OutputHandler::OutputHandler() : OdfDocumentHandler(),
	mbIsTagOpened(false)
{
}

void OutputHandler::startElement(const char *psName, const WPXPropertyList &xPropList)
{
	if (mbIsTagOpened)
	{
        m_array->append(">");
		mbIsTagOpened = false;
	}
    m_array->append("<");
    m_array->append(psName);
        WPXPropertyList::Iter i(xPropList);
        for (i.rewind(); i.next(); )
        {
                // filter out libwpd elements
                if (strncmp(i.key(), "libwpd", 6) != 0)
		{
            m_array->append(" ");
            m_array->append(i.key());
            m_array->append("=\"");
            m_array->append(i()->getStr().cstr());
            m_array->append("\"");
		}

        }
	mbIsTagOpened = true;
	msOpenedTagName.sprintf("%s", psName);
}

void OutputHandler::endElement(const char *psName)
{
	if (mbIsTagOpened)
	{
		if( msOpenedTagName == psName )
		{
            m_array->append("/>");
			mbIsTagOpened = false;
		}
		else // should not happen, but handle it
		{
            m_array->append(">");
            m_array->append("</");
            m_array->append(psName);
            m_array->append(">");
			mbIsTagOpened = false;
		}
	}
	else
	{
        m_array->append("</");
        m_array->append(psName);
        m_array->append(">");
		mbIsTagOpened = false;
	}
}

void OutputHandler::characters(const WPXString &sCharacters)
{
	if (mbIsTagOpened)
	{
        m_array->append(">");
		mbIsTagOpened = false;
	}
	WPXString sEscapedCharacters(sCharacters, true);
	if (sEscapedCharacters.len() > 0)
        m_array->append(sEscapedCharacters.cstr());
}

void OutputHandler::endDocument()
{
	if (mbIsTagOpened)
	{
        m_array->append(">");
		mbIsTagOpened = false;
	}
}

QByteArray OutputHandler::array() const
{
    return *m_array;
}
