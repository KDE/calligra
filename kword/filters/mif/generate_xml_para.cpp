/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml_para.h"
#include "treebuild_para.h"
#include "treebuild_general.h"
#include "treebuild_textrect.h"
#include "generate_xml_page.h"
#include "unitconv.h"

#include <algorithm>
#include <fstream>
#include <string>

extern ofstream xmloutstr;

static string paratext;
static bool paragraphOpen = false;

class generate_xml_paraline_element
{
friend class generate_xml_para_element;

public:
	void operator()( const ParaLineElement* ple );
	static void out_begin();
	static void out_end();

private:
	string find_and_replace( const string& search,
							 const string& replace,
							 const string& source );
	static void openParagraph();
	static void closeParagraph();
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
	paratext = "";
}


void generate_xml_para_element::out_end()
{
	xmloutstr << "    <TEXT value=\"" << paratext << "\"/>" << endl;
	generate_xml_paraline_element::closeParagraph();
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
		if( !paragraphOpen ) {
			openParagraph();
		}
		// Convert some special characters in the string.
		string outstring = ple->plestring()->value(); 
		paratext += outstring;
		break;
	}
	case ParaLineElement::T_TextRectID:
		{
			// Close previous paragraph if necessary.
			if( paragraphOpen )
				closeParagraph();
			// When a TextRectID is present, mark a new frame and get the
			// positions from the TextRect structure.
			int textrectid = ple->textrectid()->id();
			TextRect* textrect = generate_xml_page::find_text_rect( textrectid );
			BRect* brect = textrect->bRect();
			xmloutstr << "   <FRAME left=\"" << point2mm( brect->x() )
					  << "\" top=\"" << point2mm( brect->y() )
					  << "\" right=\"" << point2mm( brect->x()+brect->width() )
					  << "\" bottom=\"" << point2mm( brect->y()+brect->height() )
					  << "\"/>" << endl;
			openParagraph();
			break;
		}
	case ParaLineElement::T_Char:
		if( !paragraphOpen ) {
			openParagraph();
		}
		// special character; output it
		xmloutstr << ple->plechar()->value();
		break;
	default:
		cerr << "Unsupported ParaLineElement " << ple->type() << '\n';
	}
}


void generate_xml_paraline_element::openParagraph()
{
	xmloutstr << "   <PARAGRAPH>" << endl;
	paragraphOpen = true;
}


void generate_xml_paraline_element::closeParagraph()
{
	xmloutstr << "   </PARAGRAPH>" << endl;
	paragraphOpen = false;
}


void generate_xml_paraline_element::out_begin()
{
}


void generate_xml_paraline_element::out_end()
{
}
