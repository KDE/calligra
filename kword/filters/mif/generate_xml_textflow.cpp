/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml_textflow.h"
#include "generate_xml_para.h"
#include "treebuild_textflow.h"
#include "treebuild_para.h"

#include <algorithm>
#include <fstream>

extern ofstream xmloutstr;

class generate_xml_textflow_element
{
public:
	void operator()( const TextFlowElement* tfe );
	static void out_begin();
	static void out_end();
};

void generate_xml_textflow::operator()( const TextFlow* tf )
{
	generate_xml_textflow_element::out_begin();
	for_each( tf->elements()->begin(), tf->elements()->end(),
			  generate_xml_textflow_element() );
	generate_xml_textflow_element::out_end();
}


void generate_xml_textflow_element::operator()( const TextFlowElement* tfe )
{
	switch( tfe->type() ) {
	case TextFlowElement::T_Para: {
		Para* para = tfe->para();
		generate_xml_para_element::out_begin();
		for_each( para->elements()->begin(), para->elements()->end(),
				 generate_xml_para_element() );
		generate_xml_para_element::out_end();
		break;
	}
	default:
		cerr << "Unsupported TextFlowElement type\n";
	};
}


void generate_xml_textflow_element::out_begin()
{
	// PENDING(kalle) Adjust attributes
	xmloutstr << "  <FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\">" << endl;
}

void generate_xml_textflow_element::out_end()
{
	xmloutstr << "  </FRAMESET>" << endl;
}
