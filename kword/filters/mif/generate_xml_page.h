/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef __GENERATE_XML_PAGE_H__
#define __GENERATE_XML_PAGE_H__

class TextRect;

class generate_xml_page
{
public:
	static TextRect* find_text_rect( int textrectid );
};


#endif
