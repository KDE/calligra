/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _GENERATE_XML_PARA_H
#define _GENERATE_XML_PARA_H

class ParaElement;

class generate_xml_para_element
{
public:
	void operator()( const ParaElement* tfe );
	static void out_begin();
	static void out_end();
};

#endif
