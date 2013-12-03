/* This file is part of the Calligra project.
 *
 * Copyright 2013 Yue Liu <yue.liu@mail.com>
 *
 * Based on writerperfect code, major Contributor(s):
 *
 *  Copyright (C) 2002-2004 William Lachance (wrlach@gmail.com)
 *  Copyright (C) 2004-2006 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * The contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#include "KoStoreHandler.h"

#include <KoStore.h>
#include <QByteArray>

KoStoreHandler::KoStoreHandler(KoStore *store)
    : m_store(store)
    , mbIsTagOpened(false)
    , msOpenedTagName()
{
}

void KoStoreHandler::startElement(const char *psName, const WPXPropertyList &xPropList)
{
    if (mbIsTagOpened)
	{
        m_store->write(">", 1);
		mbIsTagOpened = false;
	}
    m_store->write("<", 1);
    m_store->write(psName);
	WPXPropertyList::Iter i(xPropList);
	for (i.rewind(); i.next(); )
	{
		// filter out libwpd elements
		if (strncmp(i.key(), "libwpd", 6) != 0)
		{
            m_store->write(" ", 1);
            m_store->write(i.key());
            m_store->write("=\"", 2);
            m_store->write(i()->getStr().cstr(), i()->getStr().len());
            m_store->write("\"", 1);
		}

	}
	mbIsTagOpened = true;
	msOpenedTagName.sprintf("%s", psName);
}

void KoStoreHandler::endElement(const char *psName)
{
	if (mbIsTagOpened)
	{
		if( msOpenedTagName == psName )
		{
            m_store->write("/>", 2);
			mbIsTagOpened = false;
		}
		else // should not happen, but handle it
		{
            m_store->write(">", 1);
            m_store->write("</", 2);
            m_store->write(psName);
            m_store->write(">", 1);
			mbIsTagOpened = false;
		}
	}
	else
	{
        m_store->write("</", 2);
        m_store->write(psName);
        m_store->write(">", 1);
		mbIsTagOpened = false;
	}
}

void KoStoreHandler::characters(const WPXString &sCharacters)
{
	if (mbIsTagOpened)
	{
        m_store->write(">", 1);
		mbIsTagOpened = false;
	}
	WPXString sEscapedCharacters(sCharacters, true);
	if (sEscapedCharacters.len() > 0)
        m_store->write(sEscapedCharacters.cstr(), sEscapedCharacters.len());
}

void KoStoreHandler::endDocument()
{
	if (mbIsTagOpened)
	{
        m_store->write(">", 1);
		mbIsTagOpened = false;
	}
}
