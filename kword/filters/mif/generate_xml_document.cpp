/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml_document.h"
#include "treebuild_document.h"
#include <algorithm>

extern DocumentElementList documentelements;

static bool findPageSize( DocumentElement* );

double generate_xml_document::paperWidth()
{
	// PENDING(kalle) Replace with hash table.
	list<DocumentElement*>::iterator iel = find_if( documentelements.begin(), 
													documentelements.end(),
													findPageSize );
	DocumentElement* el = *iel;
	DocumentPageSize* pagesize = el->pageSize();
	// PENDING(kalle) Throw error if 0.
	return pagesize->width();
}

double generate_xml_document::paperHeight()
{
	// PENDING(kalle) Replace with hash table.
	list<DocumentElement*>::iterator iel = find_if( documentelements.begin(), 
													documentelements.end(),
													findPageSize );
	DocumentElement* el = *iel;
	DocumentPageSize* pagesize = el->pageSize();
	// PENDING(kalle) Throw error if 0.
	return pagesize->height();
}


bool findPageSize( DocumentElement* el )
{
	if( el->type() == DocumentElement::T_DocumentPageSize )
		return true;
	else
		return false;
}
