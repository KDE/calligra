/* $Id$
 *
 * This file is part of MIFParse.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "backend.h"

#include <fstream>
#include <string>
#include <qstring.h>

void start_file()
{
}

void end_file()
{
}

/*!
 * Yacc somehow includes text from the original source file in the
 * qstring, and we don't want the quotation marks anyway.
 */
QString fixup_singlequoted_string( const char* in )
{
	string work = in;
	int startpos = work.find_first_of( '`' );
	int endpos = work.find_first_of( '\'', startpos+1 );

	string out = work.substr( startpos+1, endpos - startpos - 1 );

	return QString( out.c_str() );
}


/*!
 * Yacc somehow includes the closing '>' in the ID name.
 */
QString fixup_id( const char* in )
{
	string work = in;
	work.replace( work.find( '>' ), 1, "" );
	return QString( work.c_str() );
}
