/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml_para.h"
#include "treebuild_para.h"
#include "treebuild_general.h"

#include <algorithm>
#include <fstream>
#include <string>

extern ofstream xmloutstr;

class generate_xml_paraline_element
{
public:
	void operator()( const ParaLineElement* ple );
	static void out_begin();
	static void out_end();

private:
	string find_and_replace( const string& search,
							 const string& replace,
							 const string& source );
};

void generate_xml_para_element::operator()( const ParaElement* pe )
{
	switch( pe->type() ) {
	case ParaElement::T_ParaLine:
		{
			generate_xml_paraline_element::out_begin();
			ParaLine* pl = pe->paraline();
			for_each( pl->elements()->begin(), pl->elements()->end(),
					  generate_xml_paraline_element() );
			generate_xml_paraline_element::out_end();
		}
		break;
	case ParaElement::T_Unique:
		// Should not be used by filters.
		break;
	default:
		cerr << "Unsupported ParaElement: " << pe->type() << '\n';
	}
}


void generate_xml_para_element::out_begin()
{
	xmloutstr << "\n<PARAGRAPH>\n";
}


void generate_xml_para_element::out_end()
{
	xmloutstr << "\n</PARAGRAPH>\n";
}


string generate_xml_paraline_element::find_and_replace( const string& search,
														 const string& replace,
														 const string& source )
{
	string::size_type pos = source.find( search ) ;
	string retval = source;
	string retval1;
	while( pos != string::npos ) {
		retval1 = retval.replace( pos, search.length(), replace );
		pos = retval.find( search );
		retval = retval1;
	};
	return retval;
}


void generate_xml_paraline_element::operator()( const ParaLineElement* ple )
{
	switch( ple->type() ) {
	case ParaLineElement::T_String: {
		// Convert some special characters in the string.
		string outstring = ple->plestring()->value(); 
// 		string outstring1 = find_and_replace( "\\xd2 ", "&ldquo;", outstring );
// 		string outstring2 = find_and_replace( "\\xd3 ", "&rdquo;", outstring1 );
// 		string outstring3 = find_and_replace( "\\xd5 ", "'", outstring2 );
		string outstring3 = outstring; 
		xmloutstr << outstring3 << endl; 
		break;
	}
	case ParaLineElement::T_TextRectID:
		/* TextRectID is used to show in which text frame on which
		   frame the text in this para line goes. Since we are not
		   converned with pages in SGML, we can ignore it.
		*/
		break;
	case ParaLineElement::T_Char:
		// special character; output it
		xmloutstr << ple->plechar()->value();
		break;
	default:
		cerr << "Unsupported ParaLineElement " << ple->type() << '\n';
	}
}


void generate_xml_paraline_element::out_begin()
{
}


void generate_xml_paraline_element::out_end()
{
	cerr << '\n';
}
