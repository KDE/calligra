/* This file is part of the Calligra project.
 *
 * Copyright 2013 Yue Liu <yue.liu@mail.com>
 *
 * Based on writerperfect code, major Contributor(s):
 *
 *  Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * The contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#include "OutputWriter.h"

#include <KoStore.h>
#include <KoStoreHandler.h>

#include <QByteArray>

#include <libwpd-stream/WPXStreamImplementation.h>

OutputWriter::OutputWriter(KoStore* store)
    : m_store(store)
{
}

OutputWriter::~OutputWriter()
{
}

bool OutputWriter::writeChildFile(const char *childFileName, const char *str)
{
    if (!m_store)
        return true;
    if (m_store->isOpen())
        return false;
    if (!m_store->open(childFileName))
		return false;
    QByteArray arr(str);
    if (m_store->write(arr) != arr.length())
        return false;
    if (!m_store->close())
		return false;
	return true;
}

bool OutputWriter::writeChildFile(const char *childFileName, const char *str, const char )
{
    if (!m_store)
        return true;
    if (m_store->isOpen())
        return false;
    if (!m_store->open(childFileName))
        return false;
    QByteArray arr(str);
    if (m_store->write(arr) != arr.length())
        return false;
    if (!m_store->close())
        return false;
    return true;
}

bool OutputWriter::writeConvertedContent(const char *childFileName, const char *inFileName, const OdfStreamType streamType)
{
	WPXFileStream input(inFileName);

    if (!_isSupportedFormat(&input))
		return false;

	input.seek(0, WPX_SEEK_SET);

    OdfDocumentHandler *pHandler = NULL;
    if (m_store && !m_store->isOpen()) {
        if (!m_store->open(childFileName))
            return false;
        pHandler = new KoStoreHandler(m_store);
    }

    bool bRetVal = _convertDocument(&input, pHandler, streamType); // no KoStore backend for flat xml

    delete pHandler;

    if (!m_store->close())
        return false;

	return bRetVal;
}
