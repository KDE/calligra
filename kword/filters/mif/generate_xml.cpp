/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml.h"
#include "generate_xml_textflow.h"
#include "generate_xml_document.h"
#include "unitconv.h"
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
	xmloutstr << "<?xml version=\"1.0\"?>\n";
	xmloutstr << "<DOC author=\"Kalle Dalheimer\" email=\"kalle@kde.org\" editor=\"KWord/MIF-Filter\" mime=\"application/x-kword\">" << endl;
	// PENDING(kalle) Don't hard-code those values

	xmloutstr << " <PAPER format=\"1\" width=\"";
	xmloutstr << point2mm( generate_xml_document::paperWidth() );
	xmloutstr << "\" height=\"";
	xmloutstr << point2mm( generate_xml_document::paperHeight() );
	xmloutstr << "\" orientation=\"0\" columns=\"1\" columnspacing=\"3\">" << endl;
	xmloutstr << " </PAPER>";
	xmloutstr << " <ATTRIBUTES processing=\"1\" standardpage=\"1\"/>"
			  << endl;
	indent += 2;

	// write out framesets tag
	xmloutstr << " <FRAMESETS>" << endl;

	// Traverse all text flow elements
	QListIterator<TextFlow> tfi( textflows );
	TextFlow* tf = tfi.current();
	while( tf ) {
		++tfi;
		generate_xml_textflow::generate( tf );
		tf = tfi.current();
	}

	// write out /framesets tag
	xmloutstr << " </FRAMESETS>" << endl;

	xmloutstr << "</DOC>";
	xmloutstr.close();
}


