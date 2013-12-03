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

#ifndef KOSTOREHANDLER_H
#define KOSTOREHANDLER_H

#include <libodfgen/OdfDocumentHandler.hxx>

class KoStore;

class KoStoreHandler : public OdfDocumentHandler
{
public:
    KoStoreHandler(KoStore *store);
	virtual void startDocument() {}
	virtual void endDocument();
	virtual void startElement(const char *psName, const WPXPropertyList &xPropList);
	virtual void endElement(const char *psName);
	virtual void characters(const WPXString &sCharacters);

private:
    KoStoreHandler(KoStoreHandler const &);
    KoStoreHandler &operator=(KoStoreHandler const &);

    KoStore *m_store;
	bool mbIsTagOpened;
	WPXString msOpenedTagName;
};
#endif // KOSTOREHANDLER_H
