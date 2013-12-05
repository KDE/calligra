/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* writerperfect
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
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

#ifndef _OUTPUTFILEHELPER_HXX
#define _OUTPUTFILEHELPER_HXX

#include <libodfgen/OdfDocumentHandler.hxx>

class WPXInputStream;
struct OutputFileHelperImpl;

class OutputFileHelper
{
public:
	OutputFileHelper(const char *outFileName, const char *password);
	virtual ~OutputFileHelper();

	bool writeChildFile(const char *childFileName, const char *str);
	bool writeChildFile(const char *childFileName, const char *str, const char compression_level);
	bool writeConvertedContent(const char *childFileName, const char *inFileName, const OdfStreamType streamType);

private:
	virtual bool _isSupportedFormat(WPXInputStream *input, const char *password) = 0;
	virtual bool _convertDocument(WPXInputStream *input, const char *password, OdfDocumentHandler *handler, const OdfStreamType streamType) = 0;
	OutputFileHelperImpl *m_impl;

private:
	OutputFileHelper(OutputFileHelper const &);
	OutputFileHelper &operator=(OutputFileHelper const &);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
