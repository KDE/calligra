/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml.h"
#include "generate_xml_textflow.h"
#include <fstream>
#include <algorithm>

ofstream xmloutstr;
unsigned int indent = 0;

void generate_xml( const char* outfile )
{
	xmloutstr.open( outfile );
	if( !xmloutstr.is_open() ) {
		cerr << "Could not open outfile " << outfile << '\n';
	}

	// generate_header
	xmloutstr << "<DOC author=\"Kalle Dalheimer\" email=\"kalle@kde.org\" editor=\"KWord/MIF-Filter\" mime=\"application/x-kword\">" << endl;
	indent += 2;

	// Traverse all text flow elements
	for_each( textflows.begin(), textflows.end(),
			  generate_xml_textflow() );

	xmloutstr.close();
	xmloutstr << "</DOC>";
}


