/* $Id$
 *
 * This file is part of MIFParse.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _BACKEND_H
#define _BACKEND_H

#include <fstream>
#include <string>
#include <qstring.h>

extern "C" {
	void start_file();
	void end_file();	
	QString fixup_singlequoted_string( const char* );
	QString fixup_id( const char* );
};

#endif
