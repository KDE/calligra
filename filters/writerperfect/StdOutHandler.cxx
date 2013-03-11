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

#include "StdOutHandler.hxx"

#include <string.h>
#include <stdio.h>

StdOutHandler::StdOutHandler() :
    mbIsTagOpened(false)
{
    printf("<?xml version=\"1.0\"?>\n");
}

void StdOutHandler::startElement(const char *psName, const WPXPropertyList &xPropList)
{
    if (mbIsTagOpened)
    {
        printf(">");
        mbIsTagOpened = false;
    }
    printf("<%s", psName);
        WPXPropertyList::Iter i(xPropList);
        for (i.rewind(); i.next(); )
        {
                // filter out libwpd elements
                if (strncmp(i.key(), "libwpd", 6) != 0)
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

void StdOutHandler::characters(const WPXString &sCharacters)
{
    if (mbIsTagOpened)
    {
        printf(">");
        mbIsTagOpened = false;
    }
        WPXString sEscapedCharacters(sCharacters, true);
    printf("%s", sEscapedCharacters.cstr());
}

void StdOutHandler::endDocument()
{
    if (mbIsTagOpened)
    {
        printf(">");
        mbIsTagOpened = false;
    }
}
