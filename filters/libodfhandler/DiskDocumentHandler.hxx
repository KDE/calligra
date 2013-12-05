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

#ifndef _DISKDOCUMENTHANDLER_H
#define _DISKDOCUMENTHANDLER_H

#include <libodfgen/OdfDocumentHandler.hxx>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef USE_GSF_OUTPUT
#include <gsf/gsf-output.h>
#else
#include "FemtoZip.hxx"
#endif

class DiskOdfDocumentHandler : public OdfDocumentHandler
{
public:
#ifdef USE_GSF_OUTPUT
	DiskOdfDocumentHandler(GsfOutput *pOutput);
#else
	DiskOdfDocumentHandler(FemtoZip *pOutput);
#endif
	virtual void startDocument() {}
	virtual void endDocument();
	virtual void startElement(const char *psName, const WPXPropertyList &xPropList);
	virtual void endElement(const char *psName);
	virtual void characters(const WPXString &sCharacters);

private:
	DiskOdfDocumentHandler(DiskOdfDocumentHandler const &);
	DiskOdfDocumentHandler &operator=(DiskOdfDocumentHandler const &);
#ifdef USE_GSF_OUTPUT
	GsfOutput *mpOutput;
#else
	FemtoZip *mpOutput;
#endif
	bool mbIsTagOpened;
	WPXString msOpenedTagName;
};
#endif

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
