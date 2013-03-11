/* writerperfect:
 *
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
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

#ifndef _OUTPUTFILEHELPER_HXX
#define _OUTPUTFILEHELPER_HXX

#include <OdfDocumentHandler.hxx>

class WPXInputStream;
class OutputFileHelperImpl;

class OutputFileHelper
{
public:
	OutputFileHelper(const char* outFileName, const char *password);
	virtual ~OutputFileHelper();

	bool writeChildFile(const char *childFileName, const char *str);
	bool writeChildFile(const char *childFileName, const char *str, const char compression_level);
	bool writeConvertedContent(const char *childFileName, const char *inFileName, const OdfStreamType streamType);

private:
	virtual bool _isSupportedFormat(WPXInputStream *input, const char *password) = 0;
	virtual bool _convertDocument(WPXInputStream *input, const char *password, OdfDocumentHandler *handler, const OdfStreamType streamType) = 0;
	OutputFileHelperImpl* m_impl;
};

#endif
