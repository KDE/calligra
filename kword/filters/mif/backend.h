/* $Id$
 *
 * This file is part of MIFParse.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _BACKEND_H
#define _BACKEND_H

#include <string>
#include <fstream>

extern "C" {
	void start_file();
	void end_file();	
	string fixup_singlequoted_string( const char* );
	string fixup_id( const char* );
};

#endif
