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
#include <qregexp.h>
#include <qstring.h>

extern ofstream xmloutstr;

static bool paragraphOpen = false;
QString outstring;

class generate_xml_paraline_element
{
friend class generate_xml_para_element;

public:
	static void generate( const ParaLineElement* ple );
	static void out_begin();
	static void out_end();

private:
	static void openParagraph();
	static void closeParagraph();
};

void generate_xml_para_element::generate( const ParaElement* pe )
{
	switch( pe->type() ) {
	case ParaElement::T_ParaLine:
		{
			generate_xml_paraline_element::out_begin();
			ParaLine* pl = pe->paraline();
			QListIterator<ParaLineElement> plei( *pl->elements() );
			ParaLineElement* ple = plei.current();
			while( ple ) {
				++plei;
				generate_xml_paraline_element::generate( ple );
				ple = plei.current();
			}
			generate_xml_paraline_element::out_end();
		}
		break;
	case ParaElement::T_Unique:
		// Should not be used by filters.
		break;
	default:
		cerr << "Unsupported ParaElement: " << pe->type() << endl;
	}
}


void generate_xml_para_element::out_begin()
{
	outstring = "";
}


void generate_xml_para_element::out_end()
{
	xmloutstr << "    <TEXT>" << endl
			  << outstring.data() << endl
			  << "    </TEXT>" << endl;
	generate_xml_paraline_element::closeParagraph();
}


void generate_xml_paraline_element::generate( const ParaLineElement* ple )
{
	switch( ple->type() ) {
	case ParaLineElement::T_String: {
		if( !paragraphOpen ) {
			openParagraph();
		}
		// Convert some special characters in the string.
		QString line = ple->plestring()->value();
		line = line.replace( QRegExp( "\\\\x8a " ), "ä" );
		line = line.replace( QRegExp( "\\\\x80 " ), "Ä" );
		line = line.replace( QRegExp( "\\\\x9a " ), "ö" );
		line = line.replace( QRegExp( "\\\\x9f " ), "ü" );
		line = line.replace( QRegExp( "\\\\xa7 " ), "ß" );
		outstring += line;
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
			xmloutstr << "   <FRAME left=\"" << brect->x()
					  << "\" top=\"" << brect->y()
					  << "\" right=\"" << ( brect->x()+brect->width() )
					  << "\" bottom=\"" << ( brect->y()+brect->height() )
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

